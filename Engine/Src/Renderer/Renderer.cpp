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
			DV_LOG(DvRenderer, err, "D3D11Device & D3D11DeviceContext ���� ����");
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
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// sRGB ���� ���ο� ���� �޶�����.
		desc.BufferDesc.RefreshRate.Denominator = 1;			// ���� ����(vsync�� ���� �޶�����?)
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// ��Ƽ ���ø� off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = s_bWindowed;
		desc.OutputWindow = Window::GetHandle();
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek���� 0�̰� �ٸ� ���� ������ ���� �ִ�...

		if (FAILED(pDxgiFactory->CreateSwapChain(s_pDevice, &desc, &s_pSwapChain)))
		{
			DV_RELEASE(s_pSwapChain);
			DV_LOG(DvRenderer, err, "D3D11SwapChain ���� ����");
			return false;
		}

		if (!updateScreenViews())
			return false;

		DV_LOG(DvRenderer, trace, "�ʱ�ȭ ����");

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

		DV_LOG(DvRenderer, trace, "�˴ٿ� ����");
	}

	void DvRenderer::SetRenderTargetSize(uint32_t width, uint32_t height)
	{
		// ���޹��� ũ�Ⱑ ��ȿ���� Ȯ�� �ʿ�

		if (s_RenerTargetSize.x == static_cast<float>(width) && s_RenerTargetSize.y == static_cast<float>(height))
			return;

		s_RenerTargetSize = { static_cast<float>(width), static_cast<float>(height) };
		createRenderTargets();	// ���� �޼��带 �и��� �ʿ䰡 �ֳ�?

		DV_LOG(DvRenderer, info, "Intermediate Resolution ���� - {0:d}x{1:d}", width, height);
	}

	
	// ResizeTarget���� width, height�� �ػ󵵸� �������� �Ѵ�.
	// �ݸ� ResizeBuffers�� Target�� ũ�⸦ �������� �� ���� Ÿ���� ũ���.
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
			DV_LOG(DvRenderer, err, "�ػ� {0:d}x{1:d} ���濡 �����Ͽ����ϴ�.", width, height);
			return;
		}

		// ResizeBuffers
		DV_RELEASE(s_pScreenRenderTargetView);		// ��Ȯ���� �ĸ� ���۸� ������ ���� �긦 ������ �ؾ� �Ѵ�.
		s_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

		if (FAILED(s_pSwapChain->ResizeBuffers(BUFFER_COUNT, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0)))
		{
			DV_LOG(DvRenderer, err, "�ĸ� ���� ũ�� ���� ����");
			return;
		}

		updateScreenViews();

		DV_LOG(DvRenderer, info, "Screen Size ���� - {0:d}x{1:d}", width, height);
	}

	bool DvRenderer::createRenderTargets()
	{
		// �ϴ��� gbuffer�� �ش�

		return true;
	}

	// 1. �ĸ���۷κ��� texture2d�� ���� rendertargetview�� �����.
	// 2. �ĸ���۷κ��� ���� ������ �̿��� depthstencil�� texture2d�� ������ �� depthstencilview�� �����.
	bool DvRenderer::updateScreenViews()
	{
		DV_RELEASE(s_pScreenRenderTargetView);
		DV_RELEASE(s_pScreenDepthStencilTexture);
		DV_RELEASE(s_pScreenDepthStencilView);

		ID3D11Texture2D* pBackbufferTexture{};
		if (FAILED(s_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture)))
		{
			DV_RELEASE(pBackbufferTexture);
			DV_LOG(DvRenderer, err, "�ĸ� ���� �ؽ��� ȹ�� ����");
			return false;
		}

		if (FAILED(s_pDevice->CreateRenderTargetView(
			static_cast<ID3D11Resource*>(pBackbufferTexture), nullptr, &s_pScreenRenderTargetView)))
		{
			DV_RELEASE(pBackbufferTexture);
			DV_RELEASE(s_pScreenRenderTargetView);
			DV_LOG(DvRenderer, err, "�ĸ� ���� ����Ÿ�ٺ� ���� ����");
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
			DV_LOG(DvRenderer, err, "�ĸ� ���� ���� ���ٽ� �ؽ��� ���� ����");
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{};
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		if (FAILED(s_pDevice->CreateDepthStencilView(
			static_cast<ID3D11Resource*>(s_pScreenDepthStencilTexture),
			&viewDesc,//nullptr,		// urho�� nullptr�� �����ߴ�. ���ó� sampler ������ �ذ���� �ʾҴ�.
			&s_pScreenDepthStencilView)))
		{
			DV_RELEASE(s_pScreenDepthStencilView);
			DV_LOG(DvRenderer, err, "�ĸ� ���� ���� ���ٽ� �� ���� ����");
			return false;
		}

		DV_LOG(DvRenderer, info, "����� ��ũ�� �� ũ�� - {0:d}x{1:d}", desc.BufferDesc.Width, desc.BufferDesc.Height);

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

		DV_LOG(Renderer, trace, "������ �ý��� �ʱ�ȭ�� �����Ͽ����ϴ�.");

		return true;
	}

	void Renderer::Shutdown()
	{
		for (auto pViewScreen : m_ViewScreens)
			DV_DELETE(pViewScreen);
		m_ViewScreens.clear();

		DV_LOG(Renderer, trace, "������ �ý��� �˴ٿ �����Ͽ����ϴ�.");
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
