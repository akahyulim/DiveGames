#include "divepch.h"
#include "Graphics.h"
#include "Shader.h"
#include "Texture2D.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Pipeline.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	Graphics* Graphics::s_pInstance = nullptr;
	constexpr LPCWSTR DV_WINCLASS_NAME = L"DIVE_WINDOW";

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = NULL;

		switch (msg)
		{
		case WM_CLOSE:
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		return Graphics::GetInstance()->MessageHandler(hWnd, msg, wParam, lParam);
	}

	Graphics::Graphics()
		: m_hInstance(NULL)
		, m_hWnd(NULL)
		, m_WindowTitle(L"DiveGames")
		, m_bFullScreen(false)
		, m_ResolutionWidth(0)
		, m_ResolutionHeight(0)
		, m_pSwapChain(nullptr)
		, m_pDevice(nullptr)
		, m_pDeviceContext(nullptr)
		, m_pDefaultRenderTargetView(nullptr)
		, m_pDefaultDepthStencilView(nullptr)
		, m_pDefaultDepthTexture(nullptr)
		, m_bVSync(false)
		, m_PrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED)
		, m_pDepthStencilView(nullptr)
		, m_bRenderTargetViewsDirty(true)
		, m_pVertexShader(nullptr)
		, m_pHullShader(nullptr)
		, m_pDomainShader(nullptr)
		, m_pPixelShader(nullptr)
		, m_pComputeShader(nullptr)
		, m_pVertexBuffer(nullptr)
		, m_pIndexBuffer(nullptr)
		, m_pDepthStencilState(nullptr)
		, m_pRasterizerState(nullptr)
		, m_pBlendState(nullptr)
		, m_bTextureDirty(true)
	{
		for (size_t i = 0; i != MAX_NUM_RENDERTARGETS; ++i)
		{
			m_RenderTargetViews[i] = nullptr;
		}

		for (uint32_t i = 0; i != static_cast<uint32_t>(eTextureUnit::Count); ++i)
		{
			m_ShaderResourceViews[i] = nullptr;
			m_SamplerStates[i] = nullptr;
		}
	}

	Graphics::~Graphics()
	{
		Shutdown();
	}

	bool Graphics::Initialize(uint32_t width, uint32_t height, bool fullScreen, bool borderless)
	{
		// 윈도우 생성
		if (!m_hWnd)
		{
			if (!createWindow(width, height, borderless))
				return false;
		}
		AdjustWindow(width, height, borderless);

		// 디바이스 & 스왑체인 생성
		if (!m_pDevice)
		{
			if (!createDevice(width, height))
				return false;
		}
		updateSwapChain(width, height);

		ClearViews(eClearFlags::Color, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
		m_pSwapChain->Present(0, 0);

		ShowWindow(m_hWnd, SW_SHOW);
		SetFocus(m_hWnd);

		DV_ENGINE_TRACE("그래픽스 시스템 초기화에 성공하였습니다.");

		return true;
	}

	void Graphics::Shutdown()
	{
		DV_RELEASE(m_pDefaultDepthTexture);
		DV_RELEASE(m_pDefaultDepthStencilView);
		DV_RELEASE(m_pDefaultRenderTargetView);

		DV_RELEASE(m_pDeviceContext);
		DV_RELEASE(m_pDeviceContext);
		DV_RELEASE(m_pSwapChain);

		::DestroyWindow(m_hWnd);
		::UnregisterClassW(DV_WINCLASS_NAME, m_hInstance);

		DV_ENGINE_TRACE("그래픽스 시스템 셧다운에 성공하였습니다.");
	}

	bool Graphics::RunWindow()
	{
		DV_ENGINE_ASSERT(IsInitialized());

		MSG msg;
		ZeroMemory(&msg, sizeof(msg));

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		return msg.message != WM_QUIT;
	}

	LRESULT Graphics::MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_SIZE:
			if (IsInitialized())
				updateSwapChain(0, 0);
			return 0;
		}
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	void Graphics::AdjustWindow(uint32_t width, uint32_t height, bool borderless)
	{
		if (!m_hWnd)
		{
			DV_ENGINE_WARN("제어할 윈도우가 존재하지 않습니다.");
			return;
		}

		DWORD style = borderless ? WS_POPUP : WS_OVERLAPPEDWINDOW;

		auto curStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
		if (curStyle != style)
			::SetWindowLongPtr(m_hWnd, GWL_STYLE, style);

		RECT rt = { 0, 0, (LONG)width, (LONG)height };
		::AdjustWindowRect(&rt, style, FALSE);

		width = rt.right - rt.left;
		height = rt.bottom - rt.top;

		int posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		::SetWindowPos(m_hWnd, NULL, posX, posY, width, height, SWP_DRAWFRAME);

		::GetWindowRect(m_hWnd, &rt);
		DV_ENGINE_INFO("WindowRect size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);
		::GetClientRect(m_hWnd, &rt);
		DV_ENGINE_INFO("ClientRect size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		ShowWindow(m_hWnd, SW_SHOW);
	}

	bool Graphics::IsInitialized()
	{
		return m_pDevice != nullptr;
	}

	void Graphics::SetWindowTitle(const std::wstring& title)
	{
		if (!m_hWnd)
			return;

		if (m_WindowTitle != title)
		{
			SetWindowText(m_hWnd, title.c_str());
			m_WindowTitle = title;
		}
	}

	void Graphics::SetFullScreen(bool enabled)
	{
		if (!m_pSwapChain)
			return;

		if (m_bFullScreen != enabled)
		{
			m_pSwapChain->SetFullscreenState(enabled, nullptr);
			m_bFullScreen = enabled;
		}

		RECT rt;
		GetClientRect(m_hWnd, &rt);
		DV_ENGINE_INFO("ClientRect size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);
	}

	void Graphics::ResizeResolution(uint32_t width, uint32_t height)
	{
		if (!m_pSwapChain)
			return;

		DXGI_MODE_DESC desc;
		desc.Width = width;
		desc.Height = height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.RefreshRate.Denominator = 1;		// 하드 코딩
		desc.RefreshRate.Numerator = 60;		// 하드 코딩
		desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		if (FAILED(m_pSwapChain->ResizeTarget(&desc)))
		{
			DV_ENGINE_ERROR("스왑체인 타겟 리사이즈에 실패하여 윈도우 크기를 변경할 수 없습니다.");
			return;
		}
	}

	bool Graphics::BeginFrame()
	{
		if (!IsInitialized())
			return false;

		resetViews();

		// Count은 constexptr로 하는게 나아보인다?
		for (uint32_t i = 0; i < (uint32_t)eTextureUnit::Count; i++)
			SetTexture((eTextureUnit)i, nullptr);

		return true;
	}

	void Graphics::EndFrame()
	{
		if (!IsInitialized())
			return;

		m_pSwapChain->Present(m_bVSync ? 1 : 0, 0);
	}

	void Graphics::SetRenderTargetView(uint32_t index, ID3D11RenderTargetView* pRenderTargetView)
	{
		// 추후 상수를 constexpr로 바꾸기
		if (index >= 4)
			return;

		if (pRenderTargetView != m_RenderTargetViews[index])
		{
			m_RenderTargetViews[index] = pRenderTargetView;
			m_bRenderTargetViewsDirty = true;
		}
	}

	// read only 때문에 조금 애매해졌다. 
	// RenderTexture를 받아야 하나...?
	void Graphics::SetDepthStencilView(ID3D11DepthStencilView* pDepthStencilView)
	{
		if (pDepthStencilView != m_pDepthStencilView)
		{
			m_pDepthStencilView = pDepthStencilView;
			m_bRenderTargetViewsDirty = true;
		}
	}

	void Graphics::ClearViews(uint8_t flags, const DirectX::XMFLOAT4& color, float depth, uint8_t stencil)
	{
		// 이걸 주석처리해버리면 SRV가 Set되지 않는 문제가 발생한다.
		prepareDraw();

		if (flags & eClearFlags::Color)
		{
			float clearColor[4] = { color.x, color.y, color.z, color.w };

			for (uint32_t i = 0; i < 4; ++i)
			{
				if (m_RenderTargetViews[i])
				{
					m_pDeviceContext->ClearRenderTargetView(m_RenderTargetViews[i], clearColor);
				}
			}
		}

		if ((flags & eClearFlags::Depth) || (flags & eClearFlags::Stencil))
		{
			if (m_pDepthStencilView)
			{
				uint32_t clearFlags = 0;
				clearFlags |= (flags & eClearFlags::Depth) ? D3D11_CLEAR_DEPTH : 0;
				clearFlags |= (flags & eClearFlags::Stencil) ? D3D11_CLEAR_STENCIL : 0;


				m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, clearFlags, depth, stencil);
			}
		}
	}

	void Graphics::SetViewport(const RECT& rt)
	{
		static D3D11_VIEWPORT viewport;
		viewport.TopLeftX = (float)rt.left;
		viewport.TopLeftY = (float)rt.top;
		viewport.Width = (float)(rt.right - rt.left);
		viewport.Height = (float)(rt.bottom - rt.top);
		viewport.MaxDepth = 1.0f;
		viewport.MinDepth = 0.0f;

		m_pDeviceContext->RSSetViewports(1, &viewport);
	}

	void Graphics::SetDepthStencilState(ID3D11DepthStencilState* pState)
	{
		if (pState != m_pDepthStencilState)
		{
			m_pDeviceContext->OMSetDepthStencilState(pState, 0);
			m_pDepthStencilState = pState;
		}
	}

	void Graphics::SetRasterizerState(ID3D11RasterizerState* pState)
	{
		if (pState != m_pRasterizerState)
		{
			m_pDeviceContext->RSSetState(pState);
			m_pRasterizerState = pState;
		}
	}

	void Graphics::SetBlendState(ID3D11BlendState* pState)
	{
		if (pState != m_pBlendState)
		{
			float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			m_pDeviceContext->OMSetBlendState(pState, blendFactor, 0xffffffff);
			m_pBlendState = pState;
		}
	}

	// shader resource가 더 잘 어울릴 듯 한데...
	void Graphics::SetTexture(eTextureUnit index, Texture* pTexture)
	{
		if (pTexture)
		{
			if (pTexture->GetShaderResourceView() != m_ShaderResourceViews[(uint32_t)index])
			{
				if (pTexture->IsMipLevelsDirty())
					pTexture->UpdateMipLevels();

				if (pTexture->IsSamplerStateDirty())
					pTexture->UpdateSamplerState();

				m_ShaderResourceViews[static_cast<size_t>(index)] = pTexture->GetShaderResourceView();
				m_SamplerStates[static_cast<size_t>(index)] = pTexture->GetSamplerState();

				m_bTextureDirty = true;
			}
		}
		else
		{
			if (m_ShaderResourceViews[static_cast<size_t>(index)])
			{
				m_ShaderResourceViews[static_cast<size_t>(index)] = nullptr;
				m_SamplerStates[static_cast<size_t>(index)] = nullptr;

				m_bTextureDirty = true;
			}
		}

		// 일단 prepareDraw에서 떼어냈다.
		m_pDeviceContext->PSSetShaderResources((uint32_t)index, 1, &m_ShaderResourceViews[(size_t)index]);
		m_pDeviceContext->PSSetSamplers((uint32_t)index, 1, &m_SamplerStates[(size_t)index]);
	}

	void Graphics::SetShader(eShaderType type, Shader* pShader)
	{
		if (pShader)
		{
			if (type != pShader->GetType())
			{
				DV_ENGINE_ERROR("셰이더 바인딩에 실패하였습니다. 잘못된 셰이더 타입으로 시도하였습니다.");
				return;
			}
		}

		switch (type)
		{
		case eShaderType::Vertex:
			m_pVertexShader = pShader;
			m_pDeviceContext->IASetInputLayout(m_pVertexShader ? m_pVertexShader->GetInputLayout() : nullptr);
			m_pDeviceContext->VSSetShader(m_pVertexShader ? m_pVertexShader->GetVertexShader() : nullptr, nullptr, 0);
			return;
		case eShaderType::Hull:
			m_pHullShader = pShader;
			m_pDeviceContext->HSSetShader(m_pHullShader ? m_pHullShader->GetHullShader() : nullptr, nullptr, 0);
			return;
		case eShaderType::Domain:
			m_pDomainShader = pShader;
			m_pDeviceContext->DSSetShader(m_pDomainShader ? m_pDomainShader->GetDomainShader() : nullptr, nullptr, 0);
			return;
		case eShaderType::Compute:
			m_pComputeShader = pShader;
			m_pDeviceContext->CSSetShader(m_pComputeShader ? m_pComputeShader->GetComputeShader() : nullptr, nullptr, 0);
			return;
		case eShaderType::Pixel:
			m_pPixelShader = pShader;
			m_pDeviceContext->PSSetShader(m_pPixelShader ? m_pPixelShader->GetPixelShader() : nullptr, nullptr, 0);
			return;
		default:
			DV_ENGINE_ERROR("셰이더 바인딩에 실패하였습니다. 지원하지 않는 타입의 셰이더로 시도하였습니다.");
			return;
		}
	}

	void Graphics::SetVertexBuffer(VertexBuffer* pBuffer)
	{
		if (pBuffer != m_pVertexBuffer)
		{
			if (pBuffer)
				m_pDeviceContext->IASetVertexBuffers(0, 1, pBuffer->GetBuffer(), pBuffer->GetStride(), pBuffer->GetOffset());
			else
				m_pDeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);

			m_pVertexBuffer = pBuffer;
		}
	}

	void Graphics::SetIndexBuffer(IndexBuffer* pBuffer)
	{
		if (pBuffer != m_pIndexBuffer)
		{
			if (pBuffer)
				m_pDeviceContext->IASetIndexBuffer(pBuffer->GetBuffer(), pBuffer->GetFormat(), 0);
			else
				m_pDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

			m_pIndexBuffer = pBuffer;
		}
	}

	void Graphics::Draw(D3D11_PRIMITIVE_TOPOLOGY topology, uint32_t vertexCount, uint32_t vertexStart)
	{
		prepareDraw();

		if (m_PrimitiveTopology != topology)
		{
			m_pDeviceContext->IASetPrimitiveTopology(topology);
			m_PrimitiveTopology = topology;
		}

		m_pDeviceContext->Draw(vertexCount, vertexStart);
	}

	void Graphics::DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY topology, uint32_t indexCount, uint32_t indexStart)
	{
		prepareDraw();

		if (m_PrimitiveTopology != topology)
		{
			m_pDeviceContext->IASetPrimitiveTopology(topology);
			m_PrimitiveTopology = topology;
		}

		m_pDeviceContext->DrawIndexed(indexCount, indexStart, 0);
	}

	bool Graphics::createWindow(uint32_t width, uint32_t height, bool borderless)
	{
		m_hInstance = GetModuleHandle(NULL);

		WNDCLASSEX wc;
		wc.style = 0;
		wc.hInstance = m_hInstance;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = DV_WINCLASS_NAME;
		wc.cbSize = sizeof(WNDCLASSEX);

		if (!RegisterClassEx(&wc))
		{
			DV_ENGINE_ERROR("윈도우클래스 등록에 실패하였습니다.");
			return false;
		}

		DWORD style = borderless ? WS_POPUP : WS_OVERLAPPEDWINDOW;

		int posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		m_hWnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			DV_WINCLASS_NAME,
			m_WindowTitle.c_str(),
			style,
			posX, posY,
			width, height,
			NULL, NULL,
			m_hInstance,
			NULL);

		if (!m_hWnd)
		{
			DV_ENGINE_ERROR("윈도우 생성에 실패하였습니다.");
			return false;
		}

		SetForegroundWindow(m_hWnd);

		return true;
	}

	bool Graphics::createDevice(uint32_t width, uint32_t height)
	{
		if (!m_pDevice)
		{
			//D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

			if (FAILED(D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				0,
				nullptr, //&featureLevel,//nullptr,
				0, //1, //0,
				D3D11_SDK_VERSION,
				&m_pDevice,
				nullptr,
				&m_pDeviceContext)))
			{
				DV_RELEASE(m_pDevice);
				DV_RELEASE(m_pDeviceContext);
				DV_ENGINE_ERROR("D3D11 장치 생성에 실패하였습니다.");
				return false;
			}
		}

		if (m_pSwapChain)
			DV_RELEASE(m_pSwapChain);

		IDXGIDevice* pDxgiDevice = nullptr;
		m_pDevice->QueryInterface(IID_IDXGIDevice, (void**)&pDxgiDevice);
		IDXGIAdapter* pDxgiAdapter = nullptr;
		pDxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&pDxgiAdapter);
		IDXGIFactory* pDxgiFactory = nullptr;
		pDxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&pDxgiFactory);

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BufferCount = 1;
		desc.BufferDesc.Width = width;
		desc.BufferDesc.Height = height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// sRGB 적용 여부에 따라 달라진다.
		desc.BufferDesc.RefreshRate.Denominator = 1;			// 추후 수정(vsync에 따라 달라진다?)
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// 멀티 샘플링 off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = !m_bFullScreen;
		desc.OutputWindow = GetWindowHandle();
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek에선 0이고 다른 값들 설정이 남아 있다...

		if (FAILED(pDxgiFactory->CreateSwapChain(m_pDevice, &desc, &m_pSwapChain)))
		{
			DV_RELEASE(m_pSwapChain);
			DV_ENGINE_ERROR("D3D11 스왑체인 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	bool Graphics::updateSwapChain(uint32_t width, uint32_t height)
	{
		if (m_ResolutionWidth == width && m_ResolutionHeight == height)
			return true;

		m_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		DV_RELEASE(m_pDefaultRenderTargetView);
		DV_RELEASE(m_pDefaultDepthTexture);
		DV_RELEASE(m_pDefaultDepthStencilView);

		m_pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		ID3D11Texture2D* pBackbufferTexture = nullptr;
		if (FAILED(m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture)))
		{
			DV_RELEASE(pBackbufferTexture);
			DV_ENGINE_ERROR("후면 버퍼 텍스쳐를 얻어오는데 실패하였습니다.");
			return false;
		}

		if (FAILED(m_pDevice->CreateRenderTargetView(
			static_cast<ID3D11Resource*>(pBackbufferTexture), nullptr, &m_pDefaultRenderTargetView)))
		{
			DV_RELEASE(pBackbufferTexture);
			DV_RELEASE(m_pDefaultRenderTargetView);
			DV_ENGINE_ERROR("후면 버퍼 렌더타겟뷰 생성에 실패하였습니다.");
			return false;
		}
		DV_RELEASE(pBackbufferTexture);

		uint32_t curWidth = width;
		uint32_t curHeight = height;

		if (width == 0 || height == 0)
		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
			m_pSwapChain->GetDesc(&swapChainDesc);

			curWidth = swapChainDesc.BufferDesc.Width;
			curHeight = swapChainDesc.BufferDesc.Height;
		}

		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = curWidth;
		texDesc.Height = curHeight;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.SampleDesc.Count = 1;// static_cast<UINT>(screenParamm_.multiSample_);
		texDesc.SampleDesc.Quality = 0;//impl->GetMultiSampleQuality(texDesc.Format, screenParamm_.multiSample_);
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pDefaultDepthTexture)))
		{
			DV_RELEASE(m_pDefaultDepthTexture);
			DV_ENGINE_ERROR("후면 버퍼 깊이 스텐실 텍스쳐 생성에 실패하였습니다.");
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
		ZeroMemory(&viewDesc, sizeof(viewDesc));
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		if (FAILED(m_pDevice->CreateDepthStencilView(
			static_cast<ID3D11Resource*>(m_pDefaultDepthTexture),
			&viewDesc,//nullptr,		// urho가 nullptr을 전달했다. 역시나 sampler 문제는 해결되지 않았다.
			&m_pDefaultDepthStencilView)))
		{
			DV_RELEASE(m_pDefaultDepthStencilView);
			DV_ENGINE_ERROR("후면 버퍼 깊이 스텐실 뷰 생성에 실패하였습니다.");
			return false;
		}

		m_ResolutionWidth = curWidth;
		m_ResolutionHeight = curHeight;

		RECT rt;
		GetClientRect(m_hWnd, &rt);
		DV_ENGINE_ASSERT(m_ResolutionWidth == (rt.right - rt.left));
		DV_ENGINE_ASSERT(m_ResolutionHeight == (rt.bottom - rt.top));
		DV_ENGINE_INFO("Resolution: {0:d} x {1:d}", m_ResolutionWidth, m_ResolutionHeight);

		resetViews();

		return true;
	}

	// 이걸 살리는 방향으로 가야할 것 같다.
	// 이름은 update or bind Pipeline 정도로 변경?
	// 그리고 드로우콜때만 해당 함수에서 호출하도록 하는 것이...
	void Graphics::prepareDraw()
	{
		if (m_bRenderTargetViewsDirty)
		{
			// 1. DepthStencilView가 존재한다면 적용 없다면 디폴트 깊이버퍼
			// 2. !깊이쓰기, DepthSTencilView가 ReadOnly라면 ReadOnly로...
			// 3. RenderTargets[0]이 비었고, DepthStencil가 없거나, 있지만 크기가 백버퍼랑 같을 때 디폴트 렌더타겟
			// => 위의 조건들이 무슨소리인지 모르겠다.

			m_pDeviceContext->OMSetRenderTargets(4, m_RenderTargetViews, m_pDepthStencilView);

			m_bRenderTargetViewsDirty = false;
		}

		// 이름은 shaderReosurce가 어울린다.
		if (m_bTextureDirty)
		{
			//m_pDeviceContext->PSSetShaderResources(0, (uint32_t)eTextureUnit::Count, m_ShaderResourceViews);
			//m_pDeviceContext->PSSetSamplers(0, (uint32_t)eTextureUnit::Count, m_SamplerStates);

			m_bTextureDirty = false;
		}

		// vertex buffer : input layout와 묶어서 처리

		// index buffer : setIndexBuffer에서 자체 처리

		// cbuffers

		// shaders : SetShader에서 처리
	}

	// beginFrame(), reiszeBackbuffer() 등에서 호출
	void Graphics::resetViews()
	{
		for (uint32_t i = 0; i < 4; i++)
			SetRenderTargetView(i, nullptr);

		SetDepthStencilView(nullptr);

		RECT rt = { 0, 0, (LONG)m_ResolutionWidth, (LONG)m_ResolutionHeight };
		SetViewport(rt);
	}

	Graphics* GetGraphics()
	{
		return Graphics::GetInstance();
	}
}