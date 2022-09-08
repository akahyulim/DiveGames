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
		// GraphicsImple 생성
		// => 별거 아니다.

		// 쉐이더 경로 및 파일 포멧 설정
		// 이외에도 기타 설정이 있다.
	}

	Graphics::~Graphics()
	{
		DV_LOG_ENGINE_DEBUG("Graphics 소멸자 호출");
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
		// 각종 states

		DV_RELEASE(m_pDefaultDepthStencilView);
		DV_RELEASE(m_pDefaultDepthStencilTexture);
		DV_RELEASE(m_pDefaultRenderTargetView);

		// 전체화면에서 스왑체인 릴리즈시 예외 발생
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

		// 최초 생성시 화면 중앙 정렬
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

		// 전체화면이라면 변경 불가.
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

	// 윈도우의 클라이언트 영역 크기를 전달.
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


	// 이건 앱에서 윈도우 크기 및 해상도를 변경하는 함수이다.
	// 일단 이 곳에서 스왑체인을 리사이즈 하면 WM_SIZE가 발생하고
	// 이를 통해 OnResizeWindow()를 통해 백퍼버와 렌더타켓을 갱신한다.
	void Graphics::ResizeWindow(int width, int height)
	{
		if (!IsInitialized())
			return;

		// 현재 크기를 제외한 값들이 하드 코딩
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
			DV_LOG_ENGINE_ERROR("스왑체인 타겟 리사이즈에 실패하였습니다.");
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

	// 1. 전체화면 상태에서 스왑체인을 릴리즈하면 예외가 발생된다.
	// 2. 지원 해상도와 매칭이 되어야 할 것 같다.
	void Graphics::SetFullScreenWindow(bool bFullScreen)
	{
		if (!IsInitialized())
			return;

		// output target이 필요하다.
		m_pSwapChain->SetFullscreenState(bFullScreen, nullptr);

		// 기존 크기에서 해상도만 바뀌는 듯 하다.
		// 알트 + 엔터로 변경해도 wm_size는 발생하지 않는다.
		// => 발생한다. 왜 인지 모르겠지만 두 번째 이후부터 발생한다.
		// 크기는 윈도우 전체 크기로 백버퍼까지 변경된다.
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

	// 현재 윈도우 생성 후 바로 최소화하면 먹통이 되는 버그가 있다.
	// 아무래도 크기를 저장하지 못했기 때문인 듯 하다?
	// => 수정한 부분이 없는데 문제가 사라졌다...
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

		// 실제로는 함수들을 타고 들어가서
		// SetScreenMode에서 윈도우, d3d11 객체를 생성한다.

		//if (!m_hWnd)
		{
			unsigned int flags = 0;
			flags |= bFullscreen ? WINDOW_FULLSCREEN : 0;
			flags |= bBorderless ? WINDOW_BORDERLESS : 0;
			flags |= bResizable ? WINDOW_RESIZABLE : 0;

			if (!WindowCreate(width, height, flags))
			{
				DV_LOG_ENGINE_ERROR("윈도우 생성에 실패하였습니다.");
				return false;
			}
		}

		// 현재 위치는 임시
		// 순서는 이게 맞다.
		createDevice(width, height);
		updateSwapChain(width, height);

		// clear
		// present

		// OnScreenModeChanged

		// 이건 내가 추가.
		ShowWindow();

		return true;
	}

	void Graphics::Clear(int flags, const DirectX::XMFLOAT4& color, float depth, float stencil)
	{
		// render target view, depth stencil view를 clear한다.

		// 일단 임시로 backbuffer만 clear
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

		// 전체화면이면서 최소화 되어 있다면 false?
		// => 가능한 조합이 아닌 듯 한데...? 

		// reset render targets?
		// => 4개짜리 배열을 전부 nullptr로 초기화한다.
		// => GBuffer일 수도 있고, 아닐 수도 있다...

		FIRE_EVENT(BeginRenderEvent());

		return true;
	}

	void Graphics::EndFrame()
	{
		if (!IsInitialized())
			return;

		FIRE_EVENT(EndRenderEvent());

		// vsync 여부를 멤버로 가져야 할 듯
		// orho는 screenParam으로 전부 가진다.
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

		// render targets들 + viewport 도 새로 만들어야 한다.
		// 그런데 orho는 reset만 한다. 아마 다른 곳에서 새로 만드나 보다.

		// 이 함수와 함께 윈도우 모드 변경이 일어나는 다른 함수(OnScreenModeChanged)는 ScreenMode 이벤트를 발생시킨다.
		// 이때 매개변수로 크기, 모드(풀 스크린, 크기 변경 가능), 모니터 등의 정보를 전달한다. 

		DV_LOG_ENGINE_INFO("윈도우 크기가 {0:d} x {1:d}로 변경되었습니다.", width, height);
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
				DV_LOG_ENGINE_ERROR("D3D11 장치 생성에 실패하였습니다.");
				return false;
			}
		}

		// 멀티 샘플 레밸 체크?

		if (m_pSwapChain)
			DV_RELEASE(m_pSwapChain);

		IDXGIDevice* pDxgiDevice = nullptr;
		m_pDevice->QueryInterface(IID_IDXGIDevice, (void**)&pDxgiDevice);
		IDXGIAdapter* pDxgiAdapter = nullptr;
		pDxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&pDxgiAdapter);
		IDXGIFactory* pDxgiFactory = nullptr;
		pDxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&pDxgiFactory);

		// 동일 해상도에서 최적화된 리플래쉬 레이트 설정
		// 일단 건너뛴다.

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BufferCount = 1;
		desc.BufferDesc.Width = (UINT)width;
		desc.BufferDesc.Height = (UINT)height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// sRGB???
		desc.BufferDesc.RefreshRate.Denominator = 1;			// 추후 수정(vsync에 따라 달라진다?)
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// 멀티 샘플링 off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = IsFullScreen() ? FALSE : TRUE;	// orho는 TRUE 고정이다. 전체화면은 여기에서 결정된다.
		desc.OutputWindow = m_hWnd;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek에선 0이고 다른 값들 설정이 남아 있다...

		HRESULT hr = pDxgiFactory->CreateSwapChain(m_pDevice, &desc, &m_pSwapChain);

		DV_RELEASE(pDxgiFactory);
		DV_RELEASE(pDxgiAdapter);
		DV_RELEASE(pDxgiDevice);

		if (FAILED(hr))
		{
			DV_RELEASE(m_pSwapChain);
			DV_LOG_ENGINE_ERROR("D3D11 스왑체인 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	// render target을 재생성하는 함수 같다.
	// 기존에 전체화면 과정에서 구현했던 함수랑 비슷하다.
	bool Graphics::updateSwapChain(int width, int height)
	{
		// 빈 렌더타겟뷰를 Set
		ID3D11RenderTargetView* pNullView = nullptr;
		m_pDeviceContext->OMSetRenderTargets(1, &pNullView, nullptr);

		// 디폴트 리소스들을 전부 릴리즈
		DV_RELEASE(m_pDefaultRenderTargetView);
		DV_RELEASE(m_pDefaultDepthStencilTexture);
		DV_RELEASE(m_pDefaultDepthStencilView);

		// 현재 렌더타겟배열, 뎁스스텐실 뷰 nullptr

		// 리사이즈 버퍼
		m_pSwapChain->ResizeBuffers(1, (UINT)width, (UINT)height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		// 백버퍼를 얻어와 디폴트 렌더타겟뷰 생성
		ID3D11Texture2D* pBackbufferTexture = nullptr;
		if (FAILED(m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture)))
		{
			DV_RELEASE(pBackbufferTexture);
			DV_LOG_ENGINE_ERROR("후면 버퍼 텍스쳐 생성에 실패하였습니다.");
			return false;
		}

		if (FAILED(m_pDevice->CreateRenderTargetView(pBackbufferTexture, nullptr, &m_pDefaultRenderTargetView)))
		{
			DV_RELEASE(m_pDefaultRenderTargetView);
			DV_LOG_ENGINE_ERROR("후면 버퍼 렌더타겟뷰 생성에 실패하였습니다.");
			return false;
		}
		DV_RELEASE(pBackbufferTexture);

		// 디폴트 뎁스스텐실 생성

		// 백버퍼 사이즈를 저장
		m_Width = width;
		m_Height = height;

		return true;
	}
}