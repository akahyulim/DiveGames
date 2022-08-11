#include "divepch.h"
#include "Graphics.h"
#include "GraphicsEvents.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"
#include "Core/EventSystem.h"

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
		//m_hInstance(NULL),
		//m_hWnd(NULL),
		m_pSwapChain(nullptr),
		m_pDevice(nullptr),
		m_pDeviceContext(nullptr)
	{
		s_pGraphics = this;

		// Constructor_d3d11()
		// GraphicsImple ����
		// => ���� �ƴϴ�.

		// ���̴� ��� �� ���� ���� ����
		// �̿ܿ��� ��Ÿ ������ �ִ�.
	}

	Graphics::~Graphics()
	{
		DV_LOG_ENGINE_DEBUG("Graphics �Ҹ��� ȣ��");
		Destroy();
	}

	bool Graphics::IsInitialized() const
	{
		if (!m_hWnd || !m_pDevice || !m_pDeviceContext)
			return false;

		return true;
	}

	void Graphics::Destroy()
	{
		// ���� states

		DV_RELEASE(m_pDefaultDepthStencilView);
		DV_RELEASE(m_pDefaultDepthStencilTexture);
		DV_RELEASE(m_pDefaultRenderTargetView);

		// ��üȭ�鿡�� ����ü�� ������� ���� �߻�
		if (IsFullScreen())
		{
			m_pSwapChain->SetFullscreenState(FALSE, nullptr);
		}
		DV_RELEASE(m_pSwapChain);
		DV_RELEASE(m_pDeviceContext);
		DV_RELEASE(m_pDevice);

		if (m_hWnd)
		{
			CloseWindow();
		}

		s_pGraphics = nullptr;
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

		return true;
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
				break;
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

	// �������� Ŭ���̾�Ʈ ���� ũ�⸦ ����.
	void Graphics::GetWindowSize(int& outWidth, int& outHeight) const
	{
		if (!m_hWnd)
			return;

		RECT rt{ 0, 0, 0, 0 };
		::GetClientRect(m_hWnd, &rt);

		outWidth = static_cast<int>(rt.right - rt.left);
		outHeight = static_cast<int>(rt.bottom - rt.top);

		DV_LOG_ENGINE_DEBUG("Window width: {0:d}, height: {1:d}", outWidth, outHeight);
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
		int a, b;
		GetWindowSize(a, b);
		DV_LOG_ENGINE_DEBUG("FullScreen Size: {0:d} x {1:d}", a, b);
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

		// �����δ� �Լ����� Ÿ�� ����
		// SetScreenMode���� ������, d3d11 ��ü�� �����Ѵ�.

		//if (!m_hWnd)
		{
			unsigned int flags = 0;
			flags |= bFullscreen ? WINDOW_FULLSCREEN : 0;
			flags |= bBorderless ? WINDOW_BORDERLESS : 0;
			flags |= bResizable ? WINDOW_RESIZABLE : 0;

			if (!WindowCreate(width, height, flags))
			{
				DV_LOG_ENGINE_ERROR("������ ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		// ���� ��ġ�� �ӽ�
		// ������ �̰� �´�.
		createDevice(width, height);
		updateSwapChain(width, height);

		// clear
		// present

		// OnScreenModeChanged

		// �̰� ���� �߰�.
		ShowWindow();

		return true;
	}

	void Graphics::Clear(int flags, const DirectX::XMFLOAT4& color, float depth, float stencil)
	{
		// render target view, depth stencil view�� clear�Ѵ�.

		// �ϴ� �ӽ÷� backbuffer�� clear
		float colors[]{ color.x, color.y, color.z, color.w };
		m_pDeviceContext->ClearRenderTargetView(m_pDefaultRenderTargetView, colors);
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

		// reset render targets?
		// => 4��¥�� �迭�� ���� nullptr�� �ʱ�ȭ�Ѵ�.
		// => GBuffer�� ���� �ְ�, �ƴ� ���� �ִ�...

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

	void Graphics::OnResizeWindow()
	{
		if (!IsInitialized())
			return;

		int width, height;
		GetWindowSize(width, height);

		if (width == m_Width && height == m_Height)
			return;

		updateSwapChain(width, height);

		// render targets�� + viewport �� ���� ������ �Ѵ�.
		// �׷��� orho�� reset�� �Ѵ�. �Ƹ� �ٸ� ������ ���� ���峪 ����.

		// �� �Լ��� �Բ� ������ ��� ������ �Ͼ�� �ٸ� �Լ�(OnScreenModeChanged)�� ScreenMode �̺�Ʈ�� �߻���Ų��.
		// �̶� �Ű������� ũ��, ���(Ǯ ��ũ��, ũ�� ���� ����), ����� ���� ������ �����Ѵ�. 

		DV_LOG_ENGINE_INFO("������ ũ�Ⱑ {0:d} x {1:d}�� ����Ǿ����ϴ�.", width, height);
	}

	bool Graphics::createDevice(int width, int height)
	{
		if (!m_pDevice)
		{
			if (FAILED(D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				0,
				nullptr,
				0,
				D3D11_SDK_VERSION,
				&m_pDevice,
				nullptr,
				&m_pDeviceContext
			)))
			{
				DV_RELEASE(m_pDevice);
				DV_RELEASE(m_pDeviceContext);
				DV_LOG_ENGINE_ERROR("D3D11 ��ġ ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		// ��Ƽ ���� ���� üũ?

		if (m_pSwapChain)
			DV_RELEASE(m_pSwapChain);

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
		desc.BufferDesc.Width = (UINT)width;
		desc.BufferDesc.Height = (UINT)height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// sRGB???
		desc.BufferDesc.RefreshRate.Denominator = 1;			// ���� ����(vsync�� ���� �޶�����?)
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// ��Ƽ ���ø� off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = IsFullScreen() ? FALSE : TRUE;	// orho�� TRUE �����̴�. ��üȭ���� ���⿡�� �����ȴ�.
		desc.OutputWindow = m_hWnd;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek���� 0�̰� �ٸ� ���� ������ ���� �ִ�...

		HRESULT hr = pDxgiFactory->CreateSwapChain(m_pDevice, &desc, &m_pSwapChain);

		DV_RELEASE(pDxgiFactory);
		DV_RELEASE(pDxgiAdapter);
		DV_RELEASE(pDxgiDevice);

		if (FAILED(hr))
		{
			DV_RELEASE(m_pSwapChain);
			DV_LOG_ENGINE_ERROR("D3D11 ����ü�� ������ �����Ͽ����ϴ�.");
			return false;
		}

		return true;
	}

	// render target�� ������ϴ� �Լ� ����.
	// ������ ��üȭ�� �������� �����ߴ� �Լ��� ����ϴ�.
	bool Graphics::updateSwapChain(int width, int height)
	{
		// �� ����Ÿ�ٺ並 Set
		ID3D11RenderTargetView* pNullView = nullptr;
		m_pDeviceContext->OMSetRenderTargets(1, &pNullView, nullptr);

		// ����Ʈ ���ҽ����� ���� ������
		DV_RELEASE(m_pDefaultRenderTargetView);
		DV_RELEASE(m_pDefaultDepthStencilTexture);
		DV_RELEASE(m_pDefaultDepthStencilView);

		// ���� ����Ÿ�ٹ迭, �������ٽ� �� nullptr

		// �������� ����
		m_pSwapChain->ResizeBuffers(1, (UINT)width, (UINT)height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		// ����۸� ���� ����Ʈ ����Ÿ�ٺ� ����
		ID3D11Texture2D* pBackbufferTexture = nullptr;
		if (FAILED(m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture)))
		{
			DV_RELEASE(pBackbufferTexture);
			DV_LOG_ENGINE_ERROR("�ĸ� ���� �ؽ��� ������ �����Ͽ����ϴ�.");
			return false;
		}

		if (FAILED(m_pDevice->CreateRenderTargetView(pBackbufferTexture, nullptr, &m_pDefaultRenderTargetView)))
		{
			DV_RELEASE(m_pDefaultRenderTargetView);
			DV_LOG_ENGINE_ERROR("�ĸ� ���� ����Ÿ�ٺ� ������ �����Ͽ����ϴ�.");
			return false;
		}
		DV_RELEASE(pBackbufferTexture);

		// ����Ʈ �������ٽ� ����

		// ����� ����� ����
		m_Width = width;
		m_Height = height;

		return true;
	}
}