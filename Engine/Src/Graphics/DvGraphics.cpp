#include "divepch.h"
#include "DvGraphics.h"
#include "Core/DvContext.h"
#include "Base/Base.h"
#include "IO/DvLog.h"

namespace Dive
{
	static DvGraphics* s_pGraphics = nullptr;

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

	DvGraphics::DvGraphics(DvContext* pContext)
		: DvObject(pContext),
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

	DvGraphics::~DvGraphics()
	{
		DV_LOG_ENGINE_DEBUG("Graphics �Ҹ��� ȣ��");
		Destroy();
	}

	bool DvGraphics::IsInitialized() const
	{
		if (!m_hWnd || !m_pDevice || !m_pDeviceContext)
			return false;

		return true;
	}

	void DvGraphics::Destroy()
	{
		// ���� states

		DV_RELEASE(m_pDefaultDepthStencilView);
		DV_RELEASE(m_pDefaultDepthStencilTexture);
		DV_RELEASE(m_pDefaultRenderTargetView);

		DV_RELEASE(m_pSwapChain);
		DV_RELEASE(m_pDeviceContext);
		DV_RELEASE(m_pDevice);

		if (m_hWnd)
		{
			CloseWindow();
		}

		s_pGraphics = nullptr;
	}

	bool DvGraphics::WindowCreate(int width, int height, unsigned int flags)
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

		m_WindowPosition.x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		m_WindowPosition.y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		m_WindowStyles = 0;
		if (flags & DV_WINDOW_BORDERLESS)
		{
			m_WindowStyles = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		}
		else
		{
			m_WindowStyles = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;

			if (flags & DV_WINDOW_RESIZABLE)
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

	LRESULT DvGraphics::MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// �̺�Ʈ �߽�
		
		switch (msg)
		{

		}

		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	void DvGraphics::CloseWindow()
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

	bool DvGraphics::RunWindow()
	{
		if (!m_hWnd)
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

	void DvGraphics::SetWindowTitle(const std::wstring& title)
	{
		m_WindowTitle = title;

		if(m_hWnd)
			::SetWindowText(m_hWnd, title.c_str());
	}

	void DvGraphics::GetWindowPosition(int& outX, int& outY) const
	{
		if (!m_hWnd)
			return;

		outX = m_WindowPosition.x;
		outY = m_WindowPosition.y;
	}

	void DvGraphics::SetWindowPosition(int x, int y)
	{
		if (!m_hWnd)
			return;

		// ��üȭ���̶�� ���� �Ұ�.
		if (m_WindowFlags & DV_WINDOW_FULLSCREEN)
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

	void DvGraphics::GetWindowSize(int& outWidth, int& outHeight) const
	{
		if (!m_hWnd)
			return;

		outWidth = m_Width;
		outHeight = m_Height;
	}

	void DvGraphics::ResizeWindow(int width, int height)
	{
		if (!m_hWnd)
			return;

		// ��üȭ���̶�� ���� �Ұ�.
		if (m_WindowFlags & DV_WINDOW_FULLSCREEN)
			return;

		RECT rt = { 0, 0, width, height };
		::AdjustWindowRect(&rt, m_WindowStyles, FALSE);

		::SetWindowPos(
			m_hWnd, 
			NULL, 
			m_WindowPosition.x, 
			m_WindowPosition.y,
			rt.right - rt.left,
			rt.bottom - rt.top,
			SWP_SHOWWINDOW);

		m_Width = rt.right - rt.left;
		m_Height = rt.bottom - rt.top;

		::GetWindowRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Window Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		::GetClientRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Client Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);
	}

	void DvGraphics::ShowWindow()
	{
		if (!m_hWnd)
			return;

		::ShowWindow(m_hWnd, SW_SHOW);
		::SetFocus(m_hWnd);
	}

	void DvGraphics::HideWindow()
	{
		if (!m_hWnd)
			return;

		::ShowWindow(m_hWnd, SW_HIDE);
	}

	// ���� ������ �߸��� ���ظ� ����� �� �ϴ�.
	// ��üȭ�� ��ü�� ����ü�� ������ �����ȴ�.
	// �׷����� �ұ��ϰ� ũ��� ���� �־�� �ϳ�...?
	// �� �� �����غ���.
	void DvGraphics::SetFullScreenWindow(bool bFullScreen)
	{
		/*
		* if (!m_hWnd)
			return;

		if (bFullscreen)
		{
			if (m_Flags & DV_WINDOW_FULLSCREEN)
				return;
			else
			{
				m_Flags |= DV_WINDOW_FULLSCREEN;

				// ���� ��Ÿ�Ϸ� ���ư��� ���� �÷��״� �������� �ʴ´�.
				if (!(m_Flags & DV_WINDOW_BORDERLESS))
				{
					m_Styles = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
				}

				int currentResolutionX = ::GetSystemMetrics(SM_CXSCREEN);
				int currentResolutionY = ::GetSystemMetrics(SM_CYSCREEN);

				RECT rt{ 0, 0, currentResolutionX, currentResolutionY };
				::AdjustWindowRect(&rt, m_Styles, FALSE);
				::SetWindowLongPtr(m_hWnd, GWL_STYLE, m_Styles);

				// ���� ��ġ, ũ��� ���ư��� ���� �ش� ������ �������� �ʴ´�.
				::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, rt.right - rt.left, rt.bottom - rt.top, SWP_SHOWWINDOW);
			}
		}
		else
		{
			if (m_Flags & DV_WINDOW_FULLSCREEN)
			{
				m_Flags &= ~DV_WINDOW_FULLSCREEN;

				if (!(m_Flags & DV_WINDOW_BORDERLESS))
				{
					m_Styles = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
					if (m_Flags & DV_WINDOW_RESIZABLE)
					{
						m_Styles |= WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
					}
				}

				RECT rt{ 0, 0, m_Size.x, m_Size.y };
				::AdjustWindowRect(&rt, m_Styles, FALSE);
				::SetWindowLongPtr(m_hWnd, GWL_STYLE, m_Styles);
				::SetWindowPos(m_hWnd, NULL, m_Position.x, m_Position.y, rt.right - rt.left, rt.bottom - rt.top, SWP_SHOWWINDOW);
			}
			else
				return;
		}

		RECT rt;
		::GetWindowRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Window Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		::GetClientRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Client Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);
		*/
	}

	void DvGraphics::SetBorderlessWindow(bool bBorderelss)
	{
		/*
		* if (!m_hWnd)
			return;

		if (!(m_Flags & DV_WINDOW_FULLSCREEN))
		{
			if (bBordered)
			{
				if (m_Flags & DV_WINDOW_BORDERLESS)
					return;

				m_Flags |= DV_WINDOW_BORDERLESS;

				m_Styles = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			}
			else
			{
				if (m_Flags & DV_WINDOW_BORDERLESS)
					m_Flags &= ~DV_WINDOW_BORDERLESS;
				else
					return;

				m_Styles = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
				if (m_Flags & DV_WINDOW_RESIZABLE)
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

	bool DvGraphics::IsFullScreen() const
	{
		return m_WindowFlags & DV_WINDOW_FULLSCREEN;
	}

	bool DvGraphics::IsBoderlessWindow() const
	{
		return m_WindowFlags & DV_WINDOW_BORDERLESS;
	}

	bool DvGraphics::IsResizable() const
	{
		return m_WindowFlags & DV_WINDOW_RESIZABLE;
	}

	bool DvGraphics::SetMode(int width, int height, bool bFullscreen, bool bBorderless, bool bResizable, bool bVSync,
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
		// ������ ���� �� D3d11Device�� �ʱ�ȭ�Ѵ�.
		// D3D11Device�� GraphicsImple�̶�� �߰� ��ü�� ���� �����Ѵ�. => ������ ������ Graphics::CreateDevice_D3D11()���� �Ѵ�.
		// ��, Renderer, Graphics, GraphicsImple�� ������ ���ƾ� �Ѵ�.
		// => �ƹ����� Graphics�� GraphicsImple�� ������ ��Ƽ �÷��������� API�� ����� ����ϱ� ������ �� ���δ�.

		//if (!m_pWindow)
		{
			unsigned int flags = 0;
			flags |= bFullscreen ? DV_WINDOW_FULLSCREEN : 0;
			flags |= bBorderless ? DV_WINDOW_BORDERLESS : 0;
			flags |= bResizable ? DV_WINDOW_RESIZABLE : 0;

			//m_pWindow = std::make_unique<DvWindow>();
			//if (!m_pWindow->Create(m_WindowTitle, m_WindowPosition.x, m_WindowPosition.y, width, height, flags))
			{
			//	m_pWindow.release();
			//	DV_LOG_ENGINE_ERROR("������ ������ �����Ͽ����ϴ�.");
			//	return false;
			}

			//m_pWindow->Show();
			if (!WindowCreate(width, height, flags))
			{
				DV_LOG_ENGINE_ERROR("������ ������ �����Ͽ����ϴ�.");
				return false;
			}

			::ShowWindow(m_hWnd, SW_SHOW);
		}

		// ���� ��ġ�� �ӽ�
		createDevice(width, height);
		updateSwapChain(width, height);

		return true;
	}

	void DvGraphics::Clear(int flags, const DirectX::XMFLOAT4& color, float depth, float stencil)
	{
		// render target view, depth stencil view�� clear�Ѵ�.

		// �ϴ� �ӽ÷� backbuffer�� clear
		float colors[]{ color.x, color.y, color.z, color.w };
		m_pDeviceContext->ClearRenderTargetView(m_pDefaultRenderTargetView, colors);
	}

	bool DvGraphics::IsDeviceLost()
	{
		if (FAILED(m_pDevice->GetDeviceRemovedReason()))
			return true;

		return false;
	}

	bool DvGraphics::BeginFrame()
	{
		if (!IsInitialized())
			return false;

		// ��üȭ���̸鼭 �ּ�ȭ �Ǿ� �ִٸ� false?
		// => ������ ������ �ƴ� �� �ѵ�...? 

		// reset render targets?
		// => 4��¥�� �迭�� ���� nullptr�� �ʱ�ȭ�Ѵ�.
		// => GBuffer�� ���� �ְ�, �ƴ� ���� �ִ�...

		DV_EVENT_FIRE(eDvEventType::BeginRender);

		return true;
	}

	void DvGraphics::EndFrame()
	{
		if (!IsInitialized())
			return;

		DV_EVENT_FIRE(eDvEventType::EndRender);

		// vsync ���θ� ����� ������ �� ��
		// orho�� screenParam���� ���� ������.
		m_pSwapChain->Present(1, 0);
	}

	// ������κ��� �̺�Ʈ�� �޾ƾ� �Ѵ�...
	void DvGraphics::OnResizeWindow()
	{
		// ũ�⸦ ���޹ްų�, Window�κ��� ���� �Ѵ�.
		int width = 0;
		int height = 0;

		// backbuffer rendertarget�� ���� �����.
		updateSwapChain(width, height);

		// render targets�� + viewport �� ���� ������ �Ѵ�.
		// �׷��� orho�� reset�� �Ѵ�. �Ƹ� �ٸ� ������ ���� ���峪 ����.
	}

	bool DvGraphics::createDevice(int width, int height)
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
	bool DvGraphics::updateSwapChain(int width, int height)
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

		RECT rt;
		::GetWindowRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Window Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		::GetClientRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Client Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		return true;
	}
}