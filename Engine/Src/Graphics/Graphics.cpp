#include "divepch.h"
#include "Graphics.h"
#include "GraphicsEvents.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Shader.h"
#include "InputLayout.h"
#include "Texture2D.h"
#include "Renderer/Model.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Core/EventSystem.h"
#include "IO/Log.h"

namespace Dive
{
	static Graphics* s_pGraphics = nullptr;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		}

		return s_pGraphics->MessageHandler(hWnd, msg, wParam, lParam);
	}

	Graphics::Graphics(Context* pContext)
		: Object(pContext),
		m_pIndexBuffer(nullptr),
		m_pVertexShader(nullptr),
		m_pPixelShader(nullptr)
	{
		s_pGraphics = this;

		// Constructor_d3d11()
		// GraphicsImple ����
		// => ���� �ƴϴ�.

		// ���̴� ��� �� ���� ���� ����
		// �̿ܿ��� ��Ÿ ������ �ִ�.

		for (unsigned int i = 0; i < MAX_RENDERTARGETS; ++i)
			m_pRenderTargets[i] = nullptr;

		m_bRenderTargetsDirty = true;

		for (unsigned int i = 0; i < MAX_VERTEX_STREAMS; ++i)
			m_pVertexBuffers[i] = nullptr;
	}

	Graphics::~Graphics()
	{
		DV_LOG_ENGINE_TRACE("Graphics �Ҹ��� ȣ��");
		Destroy();
	}

	// ���� ������ ���� �� �ٷ� �ּ�ȭ�ϸ� ������ �Ǵ� ���װ� �ִ�.
	// �ƹ����� ũ�⸦ �������� ���߱� ������ �� �ϴ�?
	// => ������ �κ��� ���µ� ������ �������...
	bool Graphics::SetMode(int width, int height, bool bFullscreen, bool bBorderless, bool bResizable, bool bVSync,
		bool tripleBuffer, int multiSample, int refreshRate)
	{
		ScreenModeParams params;
		params.bFullScreen = bFullscreen;
		params.bBorderless = bBorderless;
		params.bResizable = bResizable;
		params.bVSync = bVSync;

		WindowModeParams windowMode;
		windowMode.width = width;
		windowMode.height = height;
		windowMode.screenModeParams = params;

		const bool maximize = (!width || !height) && !params.bFullScreen && !params.bBorderless && params.bResizable;

		unsigned int flags = 0;
		flags |= bFullscreen ? WINDOW_FULLSCREEN : 0;
		flags |= bBorderless ? WINDOW_BORDERLESS : 0;
		flags |= bResizable ? WINDOW_RESIZABLE : 0;

		if (!WindowCreate(width, height, flags))
		{
			DV_LOG_ENGINE_ERROR("������ ������ �����Ͽ����ϴ�.");
			return false;
		}

		if (!CreateDeviceAndSwapChain())
			return false;

		if (!UpdateSwapChain())
			return false;

		// ��ġ�� �ָ����?
		RegisterGraphicsObject(m_pContext);

		// clear
		// present

		// OnScreenModeChanged

		// �̰� ���� �߰�.
		ShowWindow();

		return true;
	}

	void Graphics::Destroy()
	{
		// ���� states

		// ��üȭ�鿡�� ����ü�� ������� ���� �߻�
		if (IsFullScreen())
		{
			m_pSwapChain->SetFullscreenState(FALSE, nullptr);
		}
		
		if (m_hWnd)
		{
			CloseWindow();
		}

		s_pGraphics = nullptr;
	}

	bool Graphics::IsInitialized() const
	{
		return (m_hWnd && m_pDevice && m_pDeviceContext);
	}

	bool Graphics::WindowCreate(int width, int height, unsigned int flags)
	{
		m_hInstance = GetModuleHandle(nullptr);
		m_WindowFlags = flags;

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
		wc.lpszClassName = WND_CLASS_NAME;
		wc.cbSize = sizeof(WNDCLASSEX);

		RegisterClassEx(&wc);

		// ���� ������ ȭ�� �߾� ����
		m_WindowPosition.x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		m_WindowPosition.y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		m_WindowStyles = 0;
		if (flags & WINDOW_BORDERLESS)
		{
			m_WindowStyles = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		}
		else
		{
			m_WindowStyles = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;

			if (flags & WINDOW_RESIZABLE)
			{
				m_WindowStyles |= WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
			}
		}

		RECT rt = { 0, 0, width, height };
		::AdjustWindowRect(&rt, m_WindowStyles, FALSE);

		m_hWnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			WND_CLASS_NAME,
			m_WindowTitle.c_str(),
			m_WindowStyles,
			m_WindowPosition.x,
			m_WindowPosition.y,
			rt.right - rt.left,
			rt.bottom - rt.top,
			NULL, NULL,
			m_hInstance,
			NULL);

		return m_hWnd;
	}

	LRESULT Graphics::MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (IsInitialized())
		{
			FIRE_EVENT(WindowEvent(hWnd, msg, wParam, lParam));

			switch (msg)
			{
			case WM_SIZE:
				OnResizeWindow();
				return 0;
			}
		}

		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	void Graphics::CloseWindow()
	{
		if (!m_hWnd)
			return;

		ShowCursor(true);

		if (IsFullScreen())
			::ChangeDisplaySettings(nullptr, 0);

		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;

		::UnregisterClassW(WND_CLASS_NAME, m_hInstance);
		m_hInstance = NULL;
	}

	bool Graphics::RunWindow()
	{
		if (!IsInitialized())
			return false;

		MSG msg;
		ZeroMemory(&msg, sizeof(msg));

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		return msg.message != WM_QUIT;
	}

	void Graphics::SetWindowTitle(const std::wstring& title)
	{
		m_WindowTitle = title;

		if(m_hWnd)
			::SetWindowText(m_hWnd, title.c_str());
	}

	void Graphics::GetWindowPosition(int& outX, int& outY) const
	{
		if (!m_hWnd)
			return;

		outX = m_WindowPosition.x;
		outY = m_WindowPosition.y;
	}

	void Graphics::SetWindowPosition(int x, int y)
	{
		if (!IsInitialized())
			return;

		// ��üȭ���̶�� ���� �Ұ�.
		if (m_WindowFlags & WINDOW_FULLSCREEN)
			return;

		RECT rt{ 0, 0, m_Width, m_Height };
		::AdjustWindowRect(&rt, m_WindowStyles, FALSE);

		::SetWindowPos(
			m_hWnd, 
			NULL, 
			x, 
			y,
			rt.right - rt.left,
			rt.bottom - rt.top,
			SWP_SHOWWINDOW);

		m_WindowPosition.x = x;
		m_WindowPosition.y = y;

		::GetWindowRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Window Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		::GetClientRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Client Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);
	}

	// �̰� �ۿ��� ������ ũ�� �� �ػ󵵸� �����ϴ� �Լ��̴�.
	// �ϴ� �� ������ ����ü���� �������� �ϸ� WM_SIZE�� �߻��ϰ�
	// �̸� ���� OnResizeWindow()�� ���� ���۹��� ����Ÿ���� �����Ѵ�.
	void Graphics::ResizeWindow(int width, int height)
	{
		if (!IsInitialized())
			return;

		// ���� ũ�⸦ ������ ������ �ϵ� �ڵ�
		DXGI_MODE_DESC desc;
		desc.Width = width;
		desc.Height = height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.RefreshRate.Denominator = 1;
		desc.RefreshRate.Numerator = 60;
		desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	
		if (FAILED(m_pSwapChain->ResizeTarget(&desc)))
		{
			DV_LOG_ENGINE_ERROR("����ü�� Ÿ�� ������� �����Ͽ����ϴ�.");
			return;
		}
	}

	void Graphics::ShowWindow()
	{
		if (!m_hWnd)
			return;

		::ShowWindow(m_hWnd, SW_SHOW);
		::SetFocus(m_hWnd);
	}

	void Graphics::HideWindow()
	{
		if (!m_hWnd)
			return;

		::ShowWindow(m_hWnd, SW_HIDE);
	}

	// 1. ��üȭ�� ���¿��� ����ü���� �������ϸ� ���ܰ� �߻��ȴ�.
	// 2. ���� �ػ󵵿� ��Ī�� �Ǿ�� �� �� ����.
	void Graphics::SetFullScreenWindow(bool bFullScreen)
	{
		if (!IsInitialized())
			return;

		// output target�� �ʿ��ϴ�.
		m_pSwapChain->SetFullscreenState(bFullScreen, nullptr);

		// ���� ũ�⿡�� �ػ󵵸� �ٲ�� �� �ϴ�.
		// ��Ʈ + ���ͷ� �����ص� wm_size�� �߻����� �ʴ´�.
		// => �߻��Ѵ�. �� ���� �𸣰����� �� ��° ���ĺ��� �߻��Ѵ�.
		// ũ��� ������ ��ü ũ��� ����۱��� ����ȴ�.
	}

	void Graphics::SetBorderlessWindow(bool bBorderelss)
	{
		/*
		* if (!m_hWnd)
			return;

		if (!(m_Flags & WINDOW_FULLSCREEN))
		{
			if (bBordered)
			{
				if (m_Flags & WINDOW_BORDERLESS)
					return;

				m_Flags |= WINDOW_BORDERLESS;

				m_Styles = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			}
			else
			{
				if (m_Flags & WINDOW_BORDERLESS)
					m_Flags &= ~WINDOW_BORDERLESS;
				else
					return;

				m_Styles = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
				if (m_Flags & WINDOW_RESIZABLE)
				{
					m_Styles |= WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
				}	
			}

			RECT rt{ 0, 0, m_Size.x, m_Size.y };
			::AdjustWindowRect(&rt, m_Styles, FALSE);
			::SetWindowLongPtr(m_hWnd, GWL_STYLE, m_Styles);
			::SetWindowPos(m_hWnd, NULL, m_Position.x, m_Position.y, rt.right - rt.left, rt.bottom - rt.top, SWP_SHOWWINDOW);
		}

		RECT rt;
		::GetWindowRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Window Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		::GetClientRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Client Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);
		*/
	}

	bool Graphics::IsFullScreen() const
	{
		return m_WindowFlags & WINDOW_FULLSCREEN;
	}

	bool Graphics::IsBoderlessWindow() const
	{
		return m_WindowFlags & WINDOW_BORDERLESS;
	}

	bool Graphics::IsResizable() const
	{
		return m_WindowFlags & WINDOW_RESIZABLE;
	}

	bool Graphics::CreateDeviceAndSwapChain(unsigned int width, unsigned int height)
	{
		if (FAILED(D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			m_pDevice.GetAddressOf(),
			nullptr,
			m_pDeviceContext.GetAddressOf()
		)))
		{
			DV_LOG_ENGINE_ERROR("Graphics::createDeviceAndSwapChain - D3D11 ��ġ ������ �����Ͽ����ϴ�.");
			return false;
		}

		// ��Ƽ ���� ���� üũ?

		IDXGIDevice* pDxgiDevice = nullptr;
		m_pDevice->QueryInterface(IID_IDXGIDevice, (void**)&pDxgiDevice);
		IDXGIAdapter* pDxgiAdapter = nullptr;
		pDxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&pDxgiAdapter);
		IDXGIFactory* pDxgiFactory = nullptr;
		pDxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&pDxgiFactory);

		// ���� �ػ󵵿��� ����ȭ�� ���÷��� ����Ʈ ����
		// �ϴ� �ǳʶڴ�.

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BufferCount = 1;
		desc.BufferDesc.Width = width;
		desc.BufferDesc.Height = height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// sRGB???
		desc.BufferDesc.RefreshRate.Denominator = 1;			// ���� ����(vsync�� ���� �޶�����?)
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// ��Ƽ ���ø� off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = IsFullScreen() ? FALSE : TRUE;	// orho�� TRUE �����̴�. ��üȭ���� ���⿡�� �����ȴ�.
		desc.OutputWindow = m_hWnd;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek���� 0�̰� �ٸ� ���� ������ ���� �ִ�...

		if(FAILED(pDxgiFactory->CreateSwapChain(m_pDevice.Get(), &desc, m_pSwapChain.GetAddressOf())))
		{
			DV_LOG_ENGINE_ERROR("Graphics::createDeviceAndSwapChain - D3D11 ����ü�� ������ �����Ͽ����ϴ�.");
			return false;
		}

		return true;
	}

	// ���޹��� ũ��� Default RenderTargetView, DepthStencilView�� �����Ѵ�.
	bool Graphics::UpdateSwapChain(unsigned int width, unsigned int height)
	{
		m_pDeviceContext->OMSetRenderTargets(0, NULL, NULL);
		m_pDefaultRTV.Reset();
		m_pDefaultDS.Reset();
		m_pDefaultDSV.Reset();

		for (unsigned int i = 0; i < MAX_RENDERTARGETS; ++i)
			m_pCurRenderTargetViews[i] = nullptr;
		m_pCurDepthStencilView = nullptr;

		m_pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		// ����۸� ���� ����Ʈ ����Ÿ�ٺ� ����
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackbufferTexture;
		if (FAILED(m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)pBackbufferTexture.GetAddressOf())))
		{
			DV_LOG_ENGINE_ERROR("Graphics::updateSwapChain - �ĸ� ���� �ؽ��� ������ �����Ͽ����ϴ�.");
			return false;
		}

		unsigned int currentWidth = width;
		unsigned int currentHeight = height;

		if (width == 0 || height == 0)
		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
			m_pSwapChain->GetDesc(&swapChainDesc);

			currentWidth = swapChainDesc.BufferDesc.Width;
			currentHeight = swapChainDesc.BufferDesc.Height;
		}

		if (FAILED(m_pDevice->CreateRenderTargetView(
			static_cast<ID3D11Resource*>(pBackbufferTexture.Get()), nullptr, m_pDefaultRTV.GetAddressOf())))
		{
			DV_LOG_ENGINE_ERROR("Graphics::updateSwapChain - �ĸ� ���� ����Ÿ�ٺ� ������ �����Ͽ����ϴ�.");
			return false;
		}

		// ����Ʈ �������ٽ� ����
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = currentWidth;
		desc.Height = currentHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;// static_cast<UINT>(screenParams_.multiSample_);
		desc.SampleDesc.Quality = 0;//impl->GetMultiSampleQuality(desc.Format, screenParams_.multiSample_);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, m_pDefaultDS.GetAddressOf())))
		{
			DV_LOG_ENGINE_ERROR("Graphics::updateSwapChain - ����� ���� ���ٽ� �ؽ��� ������ �����Ͽ����ϴ�.");
			return false;
		}

		if (FAILED(m_pDevice->CreateDepthStencilView(
			static_cast<ID3D11Resource*>(m_pDefaultDS.Get()),
			nullptr,
			m_pDefaultDSV.GetAddressOf())))
		{
			DV_LOG_ENGINE_ERROR("Graphics::updateSwapChain - ����� ���� ���ٽ� �� ������ �����Ͽ����ϴ�.");
			return false;
		}

		m_Width = static_cast<int>(currentWidth);
		m_Height = static_cast<int>(currentHeight);

		ResetRenderTargets();

		m_bRenderTargetsDirty = true;

		return true;
	}

	bool Graphics::IsDeviceLost()
	{
		if (FAILED(m_pDevice->GetDeviceRemovedReason()))
			return true;

		return false;
	}

	bool Graphics::BeginFrame()
	{
		if (!IsInitialized())
			return false;

		// ��üȭ���̸鼭 �ּ�ȭ �Ǿ� �ִٸ� false?
		// => ������ ������ �ƴ� �� �ѵ�...? 

		ResetRenderTargets();

		// SetTexture
		// 16���� ���� nullptr�� �Ѵ�.
		// ��� ����ϴ� �ؽ��������� �𸣰ڴ�.
		// => shader resource view ����. ��, �𵨿� ���� �ؽ��� ���̴�.

		FIRE_EVENT(BeginRenderEvent());

		return true;
	}

	void Graphics::EndFrame()
	{
		if (!IsInitialized())
			return;

		FIRE_EVENT(EndRenderEvent());

		// vsync ���θ� ����� ������ �� ��
		// orho�� screenParam���� ���� ������.
		m_pSwapChain->Present(1, 0);
	}

	// flags�� clear target flag��.
	// �� ���� �����ϰ� enum���� ������ �� �� �ϴ�.
	void Graphics::Clear(int flags, const DirectX::XMFLOAT4& color, float depth, int stencil)
	{
		prepareDraw();

		// render target view clear
		//if(flags & eClearTarget::Color)
		{
			float colors[]{ color.x, color.y, color.z, color.w };
			for (int i = 0; i < 4; ++i)
			{
				if (m_pCurRenderTargetViews[i])
					m_pDeviceContext->ClearRenderTargetView(m_pCurRenderTargetViews[i], colors);
			}
		}

		// depth stencil view clear
		//if(flags & (eClearTarget::Depth | eClearTarget::Stencil))
		{

		}
	}

	void Graphics::Draw(D3D11_PRIMITIVE_TOPOLOGY type, unsigned int vertexCount, unsigned int vertexStart)
	{
		if (!vertexCount)
			return;

		prepareDraw();

		if (m_PrimitiveType != type)
		{
			m_pDeviceContext->IASetPrimitiveTopology(type);
			m_PrimitiveType = type;
		}

		m_pDeviceContext->Draw(vertexCount, vertexStart);
	}

	void Graphics::DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY type, unsigned int indexCount, unsigned int indexStart, unsigned int baseVertexIndex)
	{
		// �� ������ VertexCount�� ���� �� ���� Ȯ���ϴ� ������ ������ �ϴ� �ǳʶ��

		prepareDraw();

		if (m_PrimitiveType != type)
		{
			m_pDeviceContext->IASetPrimitiveTopology(type);
			m_PrimitiveType = type;
		}

		m_pDeviceContext->DrawIndexed(indexCount, indexStart, baseVertexIndex);
	}

	void Graphics::SetDepthStencil(Texture2D* pTexture)
	{
		if (m_pDepthStencil != pTexture)
			m_pDepthStencil = pTexture;

		m_bRenderTargetsDirty = true;

		// �� ������ raseterizer dirty�� true�� �Ѵ�. �׷��� �Ʒ� �Լ����� ���Ѵ�....
	}

	ID3D11RenderTargetView* Graphics::GetRenderTarget(unsigned int index) const
	{
		return index < MAX_RENDERTARGETS ? m_pRenderTargets[index]->GetRenderTargetView() : nullptr;
	}

	// Textur2D�� �ƴ϶� Texture�� �޾ƾ� ���� ������?
	void Graphics::SetRenderTarget(unsigned int index, Texture2D* pTexture)
	{
		if (index >= MAX_RENDERTARGETS)
			return;

		if (pTexture != m_pRenderTargets[index])
		{
			m_pRenderTargets[index] = pTexture;
			m_bRenderTargetsDirty = true;

			if (pTexture)
			{
				if (pTexture->GetMipmapCount() > 1)
					pTexture->SetMipLevelsDirty();

				// �̿ܿ��� ��� ó���� �� �ϴµ�... �� �𸣰ڴ�.
				// �׷��� SetTexture�� ȣ���ؾ߸� �Ӹ��� �������ٵ�...
			}
		}
	}

	void Graphics::ResetRenderTargets()
	{
		// render targets
		for (unsigned int i = 0; i < MAX_RENDERTARGETS; ++i)
			SetRenderTarget(i, nullptr);

		// depth stencil
		SetDepthStencil(nullptr);

		// viewport
		RECT viewportRect;
		viewportRect.left = 0;
		viewportRect.right = static_cast<LONG>(m_Width);
		viewportRect.top = 0;
		viewportRect.bottom = static_cast<LONG>(m_Height);

		SetViewportRect(viewportRect);
	}

	RECT Graphics::GetViewportRect() const
	{
		RECT rect;
		rect.left = static_cast<LONG>(m_Viewport.TopLeftX);
		rect.right = rect.left + static_cast<LONG>(m_Viewport.Width);
		rect.top = static_cast<LONG>(m_Viewport.TopLeftY);
		rect.bottom = rect.top + static_cast<LONG>(m_Viewport.Height);

		return rect;
	}

	void Graphics::SetViewport(const D3D11_VIEWPORT& viewport)
	{
		m_Viewport = viewport;

		m_pDeviceContext->RSSetViewports(1, &m_Viewport);
	}

	void Graphics::SetViewportRect(const RECT& rect)
	{
		D3D11_VIEWPORT viewport;

		viewport.TopLeftX = static_cast<float>(rect.left);
		viewport.TopLeftY = static_cast<float>(rect.top);

		viewport.Width = static_cast<float>(rect.right - rect.left);
		viewport.Height = static_cast<float>(rect.bottom - rect.top);

		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		SetViewport(viewport);
	}

	VertexBuffer* Graphics::GetVertexBuffer(unsigned int index) const
	{
		return index < MAX_VERTEX_STREAMS ? m_pVertexBuffers[index] : nullptr;
	}

	void Graphics::SetVertexBuffer(VertexBuffer* pBuffer)
	{
		static std::vector<VertexBuffer*> vertexBuffers(1);
		vertexBuffers[0] = pBuffer;
		SetVertexBuffers(vertexBuffers);
	}

	void Graphics::SetVertexBuffers(const std::vector<VertexBuffer*>& buffers, unsigned int instanceOffset)
	{
		// �ϴ� ���� �ϳ���
		m_pVertexBuffers[0] = buffers[0];

		unsigned int stride = m_pVertexBuffers[0]->GetVertexSize();
		unsigned int offset = 0;
		auto pVb = m_pVertexBuffers[0]->GetBuffer();

		m_pDeviceContext->IASetVertexBuffers(0, 1, &pVb, &stride, &offset);
	}

	void Graphics::SetIndexBuffer(IndexBuffer* pBuffer)
	{
		if (m_pIndexBuffer != pBuffer)
		{
			if (pBuffer)
			{
				m_pDeviceContext->IASetIndexBuffer(
					pBuffer->GetBuffer(),
					pBuffer->GetIndexSize() == sizeof(unsigned short) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT,
					0);
			}
			else
				m_pDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

			m_pIndexBuffer = pBuffer;
		}
	}

	// ���� ����� �� ������ �����ؾ� �Ѵ�.
	void Graphics::SetShaders(Shader* pVertexShader, Shader* pPixelShader)
	{
		if (m_pVertexShader == pVertexShader && m_pPixelShader == pPixelShader)
			return;

		if (m_pVertexShader != pVertexShader)
		{
			// ���޹��� pVertexShader�� ����ִٸ� ���� �����.

			m_pDeviceContext->VSSetShader(
				(ID3D11VertexShader*)(pVertexShader ? pVertexShader->GetShader() : nullptr),
				nullptr, 0);

			m_pVertexShader = pVertexShader;
		}

		if (m_pPixelShader != pPixelShader)
		{
			// ���� ���̴��� ����ٸ� ���� �����.

			m_pDeviceContext->PSSetShader(
				(ID3D11PixelShader*)(pPixelShader ? pPixelShader->GetShader() : nullptr),
				nullptr, 0);

			m_pPixelShader = pPixelShader;
		}

		// ���� ���̴� �Ķ���Ϳ� ������۸� ������Ʈ
		if (m_pVertexShader && m_pPixelShader)
		{

		}
	}

	bool Graphics::LoadShaders()
	{
		if (!m_pDefaultVS)
		{
			m_pDefaultVS = new Shader(m_pContext, eShaderType::Vertex);
			if (!m_pDefaultVS->Compile("../Output/CoreData/Shaders/Color.hlsl"))
				return false;
		}

		if (!m_pDefaultPS)
		{
			m_pDefaultPS = new Shader(m_pContext, eShaderType::Pixel);
			if (!m_pDefaultPS->Compile("../Output/CoreData/Shaders/Color.hlsl"))
				return false;
		}

		// constant buffer
		m_pMatrixBuffer = new ConstantBuffer(m_pContext);
		m_pMatrixBuffer->CreateBuffer<MatrixBufferType>();

		// �ϴ� ���� �� �ٷ� ��������� ���� �ٲ� �� �ִ�.
		// depthstencilstate
		{

		}
		// rssetstate
		{
			ID3D11RasterizerState* pRasterizerState = nullptr;

			D3D11_RASTERIZER_DESC desc;
			desc.AntialiasedLineEnable = false;
			desc.CullMode = D3D11_CULL_BACK;
			desc.DepthBias = 0;
			desc.DepthBiasClamp = 0.0f;
			desc.DepthClipEnable = true;
			desc.FillMode = D3D11_FILL_SOLID;
			desc.FrontCounterClockwise = false;
			desc.MultisampleEnable = false;
			desc.ScissorEnable = false;
			desc.SlopeScaledDepthBias = 0.0f;

			// Create the rasterizer state from the description we just filled out.
			auto result = m_pDevice->CreateRasterizerState(&desc, &pRasterizerState);
			if (FAILED(result))
			{
				return false;
			}

			// Now set the rasterizer state.
			m_pDeviceContext->RSSetState(pRasterizerState);
		}

		return true;
	}

	void Graphics::SetDefaultShader()
	{
		if (!m_pDefaultVS || !m_pDefaultPS)
			return;

		auto pVs = static_cast<ID3D11VertexShader*>(m_pDefaultVS->GetShader());
		m_pDeviceContext->VSSetShader(pVs, nullptr, 0);

		auto pPs = static_cast<ID3D11PixelShader*>(m_pDefaultPS->GetShader());
		m_pDeviceContext->PSSetShader(pPs, nullptr, 0);

		if (!m_pDefaultIL)
		{
			auto pVb = m_pVertexBuffers[0];
			m_pDefaultIL = new InputLayout(m_pContext, pVb, m_pDefaultVS);
		}

		m_pDeviceContext->IASetInputLayout(m_pDefaultIL->GetInputLayout());
	}

	void Graphics::OnResizeWindow()
	{
		if (!IsInitialized())
			return;

		UpdateSwapChain();

		// render targets�� + viewport �� ���� ������ �Ѵ�.
		// �׷��� orho�� reset�� �Ѵ�. �Ƹ� �ٸ� ������ ���� ���峪 ����.

		// �� �Լ��� �Բ� ������ ��� ������ �Ͼ�� �ٸ� �Լ�(OnScreenModeChanged)�� ScreenMode �̺�Ʈ�� �߻���Ų��.
		// �̶� �Ű������� ũ��, ���(Ǯ ��ũ��, ũ�� ���� ����), ����� ���� ������ �����Ѵ�.
	}

	// �̸��� �ٲ����� ���ڴ�.
	// ���� bind�� �����ϴ� �����̴�.
	void Graphics::prepareDraw()
	{
		// �ӽ�
		// world, view, proj ����� constant buffer�� vs�� �����ؾ� �Ѵ�.
		// world: Model GameObject�� Transform���κ��� ȹ��.
		// view & proj: Camera Component�κ��� ȹ��.
		// constant buffer�� ���� �� bind ���� ������ ��ġ�� ã�ƾ� �Ѵ�.
		{
			using namespace DirectX;

			// world
			auto worldMatrix = DirectX::XMMatrixIdentity();

			// view
			XMFLOAT3 up, position, lookAt;
			XMVECTOR upVector, positionVector, lookAtVector;
			float yaw, pitch, roll;
			XMMATRIX rotationMatrix;


			// Setup the vector that points upwards.
			up.x = 0.0f;
			up.y = 1.0f;
			up.z = 0.0f;

			// Load it into a XMVECTOR structure.
			upVector = XMLoadFloat3(&up);

			// Setup the position of the camera in the world.
			position.x = 0.0f;
			position.y = 0.0f;
			position.z = -5.0f;

			// Load it into a XMVECTOR structure.
			positionVector = XMLoadFloat3(&position);

			// Setup where the camera is looking by default.
			lookAt.x = 0.0f;
			lookAt.y = 0.0f;
			lookAt.z = 1.0f;

			// Load it into a XMVECTOR structure.
			lookAtVector = XMLoadFloat3(&lookAt);

			// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
			pitch = 0 * 0.0174532925f;
			yaw = 0 * 0.0174532925f;
			roll = 0 * 0.0174532925f;

			// Create the rotation matrix from the yaw, pitch, and roll values.
			rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

			// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
			lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
			upVector = XMVector3TransformCoord(upVector, rotationMatrix);

			// Translate the rotated camera position to the location of the viewer.
			lookAtVector = XMVectorAdd(positionVector, lookAtVector);

			// Finally create the view matrix from the three updated vectors.
			auto viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

			// proj
			float fieldOfView = 3.141592654f / 4.0f;
			float screenAspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);
			auto projMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.1f, 1000.0f);

			// constant buffer�� ����� vs�� �����ؾ� �Ѵ�.
			worldMatrix = XMMatrixTranspose(worldMatrix);
			viewMatrix = XMMatrixTranspose(viewMatrix);
			projMatrix = XMMatrixTranspose(projMatrix);

			MatrixBufferType matrixBuffer;
			matrixBuffer.world = worldMatrix;
			matrixBuffer.view = viewMatrix;
			matrixBuffer.proj = projMatrix;

			m_pMatrixBuffer->Update(static_cast<void*>(&matrixBuffer));
			auto pConstantBuffer = m_pMatrixBuffer->GetBuffer();

			unsigned int bufferNumber = 0;
			m_pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, m_pMatrixBuffer->GetBufferAddressOf());
		}

		if (m_bRenderTargetsDirty)
		{
			// �̷����ϸ� ID3D11DepthstencilView�� ������ ����ϰ� �ȴ�.
			m_pCurDepthStencilView = (m_pDepthStencil && m_pDepthStencil->GetUsage() == eTextureUsage::DepthStencil) ?
				m_pDepthStencil->GetDepthStencilView() : m_pDefaultDSV.Get();
			// write�� �ƴ϶�� read only

			for (unsigned int i = 0; i < MAX_RENDERTARGETS; ++i)
			{
				m_pCurRenderTargetViews[i] = (m_pRenderTargets[i] && m_pRenderTargets[i]->GetUsage() == eTextureUsage::RenderTarget) ?
					m_pRenderTargets[i]->GetRenderTargetView() : nullptr;
			}

			if (!m_pRenderTargets[0] && (!m_pDepthStencil ||
				(m_pDepthStencil && m_pDepthStencil->GetWidth() == m_Width && m_pDepthStencil->GetHeight() == m_Height)))
			{
				m_pCurRenderTargetViews[0] = m_pDefaultRTV.Get();
			}

			m_pDeviceContext->OMSetRenderTargets(MAX_RENDERTARGETS, &m_pCurRenderTargetViews[0], nullptr);// m_pCurDepthStencilView);
			
			m_bRenderTargetsDirty = false;
		}
	
		// shader resources + samplers

		// vertex buffer + inputlayout

		// blend state

		// depthstencil state

		// rasterizer state

		// RSSetScissorRect
	}

	void RegisterGraphicsObject(Context* pContext)
	{
		Texture2D::RegisterObject(pContext);
		Model::RegisterObject(pContext);
	}
}