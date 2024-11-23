#include "divepch.h"
#include "Renderer.h"
#include "ViewScreen.h"
#include "Graphics/Graphics.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/Shader.h"
#include "Core/CoreDefs.h"
#include "Core/Window.h"

namespace Dive
{
	constexpr uint32_t BUFFER_COUNT = 2;

	IDXGISwapChain* DvRenderer::s_pSwapChain = nullptr;
	ID3D11Device* DvRenderer::s_pDevice = nullptr;
	ID3D11DeviceContext* DvRenderer::s_pDeviceContext = nullptr;
	ID3D11RenderTargetView* DvRenderer::s_pScreenRenderTargetView = nullptr;
	ID3D11Texture2D* DvRenderer::s_pScreenDepthStencilTexture = nullptr;
	ID3D11DepthStencilView* DvRenderer::s_pScreenDepthStencilView = nullptr;

	DirectX::XMFLOAT2 DvRenderer::s_RenerTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 DvRenderer::s_ScreenSize = { 0.0f, 0.0f };
	BOOL DvRenderer::s_bWindowed = TRUE;

	bool DvRenderer::Initialize()
	{
		s_ScreenSize = { static_cast<float>(Window::GetWidth()), static_cast<float>(Window::GetHeight()) };

		if (FAILED(::D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			nullptr, //&featureLevel,//nullptr,
			0, //1, //0,
			D3D11_SDK_VERSION,
			&s_pDevice,
			nullptr,
			&s_pDeviceContext)))
		{
			DV_RELEASE(s_pDevice);
			DV_RELEASE(s_pDeviceContext);
			DV_LOG(DvRenderer, err, "D3D11Device & D3D11DeviceContext 생성 실패");
			return false;
		}

		IDXGIDevice* pDxgiDevice{};
		s_pDevice->QueryInterface(IID_IDXGIDevice, (void**)&pDxgiDevice);
		IDXGIAdapter* pDxgiAdapter{};
		pDxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&pDxgiAdapter);
		IDXGIFactory* pDxgiFactory{};
		pDxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&pDxgiFactory);

		DXGI_SWAP_CHAIN_DESC desc{};
		desc.BufferCount = BUFFER_COUNT;
		desc.BufferDesc.Width = Window::GetWidth();
		desc.BufferDesc.Height = Window::GetHeight();
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// sRGB 적용 여부에 따라 달라진다.
		desc.BufferDesc.RefreshRate.Denominator = 1;			// 추후 수정(vsync에 따라 달라진다?)
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// 멀티 샘플링 off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = s_bWindowed;
		desc.OutputWindow = Window::GetHandle();
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek에선 0이고 다른 값들 설정이 남아 있다...

		if (FAILED(pDxgiFactory->CreateSwapChain(s_pDevice, &desc, &s_pSwapChain)))
		{
			DV_RELEASE(s_pSwapChain);
			DV_LOG(DvRenderer, err, "D3D11SwapChain 생성 실패");
			return false;
		}

		if (!updateScreenViews())
			return false;

		DV_LOG(DvRenderer, trace, "초기화 성공");

		return true;
	}

	void DvRenderer::Shutdown()
	{
		DV_RELEASE(s_pScreenDepthStencilView);
		DV_RELEASE(s_pScreenDepthStencilTexture);
		DV_RELEASE(s_pScreenRenderTargetView);

		DV_RELEASE(s_pDeviceContext);
		DV_RELEASE(s_pDevice);
		DV_RELEASE(s_pSwapChain);

		DV_LOG(DvRenderer, trace, "셧다운 성공");
	}

	void DvRenderer::SetRenderTargetSize(uint32_t width, uint32_t height)
	{
		// 전달받은 크기가 유효한지 확인 필요

		if (s_RenerTargetSize.x == static_cast<float>(width) && s_RenerTargetSize.y == static_cast<float>(height))
			return;

		s_RenerTargetSize = { static_cast<float>(width), static_cast<float>(height) };
		createRenderTargets();	// 굳이 메서드를 분리할 필요가 있나?

		DV_LOG(DvRenderer, info, "Intermediate Resolution 변경 - {0:d}x{1:d}", width, height);
	}

	
	// ResizeTarget에서 width, height는 해상도를 기준으로 한다.
	// 반면 ResizeBuffers는 Target의 크기를 기준으로 한 렌더 타겟의 크기다.
	void DvRenderer::ChangeScreenSize(uint32_t width, uint32_t height)
	{
		if (!s_pSwapChain)
			return;

		if (s_ScreenSize.x == static_cast<float>(width) && s_ScreenSize.y == static_cast<float>(height))
			return;

		s_ScreenSize = { static_cast<float>(width), static_cast<float>(height) };

		// ResizeTarget
		DXGI_MODE_DESC desc{};
		desc.Width = width;
		desc.Height = height;
		desc.RefreshRate.Numerator = 60;
		desc.RefreshRate.Denominator = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		if (FAILED(s_pSwapChain->ResizeTarget(&desc)))
		{
			DV_LOG(DvRenderer, err, "해상도 {0:d}x{1:d} 변경에 실패하였습니다.", width, height);
			return;
		}

		// ResizeBuffers
		DV_RELEASE(s_pScreenRenderTargetView);		// 정확히는 후면 버퍼를 참조해 만든 얘를 릴리즈 해야 한다.
		s_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

		if (FAILED(s_pSwapChain->ResizeBuffers(BUFFER_COUNT, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0)))
		{
			DV_LOG(DvRenderer, err, "후면 버퍼 크기 갱신 실패");
			return;
		}

		updateScreenViews();

		DV_LOG(DvRenderer, info, "Screen Size 변경 - {0:d}x{1:d}", width, height);
	}

	bool DvRenderer::createRenderTargets()
	{
		// 일단은 gbuffer만 해당

		return true;
	}

	// 1. 후면버퍼로부터 texture2d를 얻어와 rendertargetview를 만든다.
	// 2. 후면버퍼로부터 얻어온 정보를 이용해 depthstencil용 texture2d를 생성한 후 depthstencilview를 만든다.
	bool DvRenderer::updateScreenViews()
	{
		DV_RELEASE(s_pScreenRenderTargetView);
		DV_RELEASE(s_pScreenDepthStencilTexture);
		DV_RELEASE(s_pScreenDepthStencilView);

		ID3D11Texture2D* pBackbufferTexture{};
		if (FAILED(s_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture)))
		{
			DV_RELEASE(pBackbufferTexture);
			DV_LOG(DvRenderer, err, "후면 버퍼 텍스쳐 획득 실패");
			return false;
		}

		if (FAILED(s_pDevice->CreateRenderTargetView(
			static_cast<ID3D11Resource*>(pBackbufferTexture), nullptr, &s_pScreenRenderTargetView)))
		{
			DV_RELEASE(pBackbufferTexture);
			DV_RELEASE(s_pScreenRenderTargetView);
			DV_LOG(DvRenderer, err, "후면 버퍼 렌더타겟뷰 생성 실패");
			return false;
		}
		DV_RELEASE(pBackbufferTexture);

		DXGI_SWAP_CHAIN_DESC desc{};
		s_pSwapChain->GetDesc(&desc);

		D3D11_TEXTURE2D_DESC texDesc{};
		texDesc.Width = desc.BufferDesc.Width;
		texDesc.Height = desc.BufferDesc.Height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.SampleDesc.Count = 1;// static_cast<UINT>(screenParams_.multiSample_);
		texDesc.SampleDesc.Quality = 0;//impl->GetMultiSampleQuality(texDesc.Format, screenParams_.multiSample_);
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if (FAILED(s_pDevice->CreateTexture2D(&texDesc, nullptr, &s_pScreenDepthStencilTexture)))
		{
			DV_RELEASE(s_pScreenDepthStencilTexture);
			DV_LOG(DvRenderer, err, "후면 버퍼 깊이 스텐실 텍스쳐 생성 실패");
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{};
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		if (FAILED(s_pDevice->CreateDepthStencilView(
			static_cast<ID3D11Resource*>(s_pScreenDepthStencilTexture),
			&viewDesc,//nullptr,		// urho가 nullptr을 전달했다. 역시나 sampler 문제는 해결되지 않았다.
			&s_pScreenDepthStencilView)))
		{
			DV_RELEASE(s_pScreenDepthStencilView);
			DV_LOG(DvRenderer, err, "후면 버퍼 깊이 스텐실 뷰 생성 실패");
			return false;
		}

		DV_LOG(DvRenderer, info, "변경된 스크린 뷰 크기 - {0:d}x{1:d}", desc.BufferDesc.Width, desc.BufferDesc.Height);

		return true;
	}

	// ===================================================================================================================================

	Renderer* Renderer::s_pInstance = nullptr;
	
	Renderer::Renderer()
		: m_pDevice(Graphics::GetInstance()->GetDevice())
		, m_RenderTargets{}
		, m_bInitialized(false)
	{
		DV_ASSERT(Renderer, m_pDevice);
	}

	Renderer::~Renderer()
	{
		Shutdown();
	}

	bool Renderer::Initialize()
	{
		if (IsInitialized())
			Shutdown();
		
		m_bInitialized = true;

		DV_LOG(Renderer, trace, "렌더러 시스템 초기화에 성공하였습니다.");

		return true;
	}

	void Renderer::Shutdown()
	{
		for (auto pViewScreen : m_ViewScreens)
			DV_DELETE(pViewScreen);
		m_ViewScreens.clear();

		DV_LOG(Renderer, trace, "렌더러 시스템 셧다운에 성공하였습니다.");
	}

	void Renderer::Update()
	{
		for (auto pViewport : m_ViewScreens)
			pViewport->Update();
	}

	void Renderer::Render()
	{
		for (auto pViewport : m_ViewScreens)
			pViewport->Render();
	}

	void Renderer::SetNumViewScreens(uint32_t count)
	{
		DV_ASSERT(Renderer, count >= 0);
		m_ViewScreens.resize(count);
	}

	ViewScreen* Renderer::GetViewScreen(uint32_t index)
	{
		DV_ASSERT(Renderer, index >= 0);
		return index < m_ViewScreens.size() ? m_ViewScreens[index] : nullptr;
	}

	void Renderer::SetViewScreen(uint32_t index, ViewScreen* pViewScreen)
	{
		DV_ASSERT(Renderer, index >= 0);
		if (index >= m_ViewScreens.size())
			m_ViewScreens.resize(index + 1);

		m_ViewScreens[index] = pViewScreen;
	}

	bool Renderer::createRenderTextures()
	{
		return true;
	}

	Renderer* GetRenderer()
	{
		return Renderer::GetInstance();
	}
}
