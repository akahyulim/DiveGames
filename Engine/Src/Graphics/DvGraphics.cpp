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
		// GraphicsImple 생성
		// => 별거 아니다.

		// 쉐이더 경로 및 파일 포멧 설정
		// 이외에도 기타 설정이 있다.
	}

	DvGraphics::~DvGraphics()
	{
		DV_LOG_ENGINE_DEBUG("Graphics 소멸자 호출");
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
		// 각종 states

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
		// 이벤트 발신
		
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

		// 전체화면이라면 변경 불가.
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

		// 전체화면이라면 변경 불가.
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

	// 이전 구현은 잘못된 이해를 기반한 듯 하다.
	// 전체화면 자체는 스왑체인 생성때 결정된다.
	// 그럼에도 불구하고 크기는 맞춰 주어야 하나...?
	// 좀 더 생각해보자.
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

				// 기존 스타일로 돌아가기 위해 플래그는 변경하지 않는다.
				if (!(m_Flags & DV_WINDOW_BORDERLESS))
				{
					m_Styles = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
				}

				int currentResolutionX = ::GetSystemMetrics(SM_CXSCREEN);
				int currentResolutionY = ::GetSystemMetrics(SM_CYSCREEN);

				RECT rt{ 0, 0, currentResolutionX, currentResolutionY };
				::AdjustWindowRect(&rt, m_Styles, FALSE);
				::SetWindowLongPtr(m_hWnd, GWL_STYLE, m_Styles);

				// 기존 위치, 크기로 돌아가기 위해 해당 변수에 저장하지 않는다.
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

		// 실제로는 함수들을 타고 들어가서
		// 윈도우 생성 후 D3d11Device를 초기화한다.
		// D3D11Device는 GraphicsImple이라는 추가 객체를 통해 관리한다. => 하지만 생성은 Graphics::CreateDevice_D3D11()에서 한다.
		// 즉, Renderer, Graphics, GraphicsImple을 구분해 놓아야 한다.
		// => 아무래도 Graphics와 GraphicsImple의 구분은 멀티 플랫폼때문에 API를 만들어 사용하기 때문인 듯 보인다.

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
			//	DV_LOG_ENGINE_ERROR("윈도우 생성에 실패하였습니다.");
			//	return false;
			}

			//m_pWindow->Show();
			if (!WindowCreate(width, height, flags))
			{
				DV_LOG_ENGINE_ERROR("윈도우 생성에 실패하였습니다.");
				return false;
			}

			::ShowWindow(m_hWnd, SW_SHOW);
		}

		// 현재 위치는 임시
		createDevice(width, height);
		updateSwapChain(width, height);

		return true;
	}

	void DvGraphics::Clear(int flags, const DirectX::XMFLOAT4& color, float depth, float stencil)
	{
		// render target view, depth stencil view를 clear한다.

		// 일단 임시로 backbuffer만 clear
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

		// 전체화면이면서 최소화 되어 있다면 false?
		// => 가능한 조합이 아닌 듯 한데...? 

		// reset render targets?
		// => 4개짜리 배열을 전부 nullptr로 초기화한다.
		// => GBuffer일 수도 있고, 아닐 수도 있다...

		DV_EVENT_FIRE(eDvEventType::BeginRender);

		return true;
	}

	void DvGraphics::EndFrame()
	{
		if (!IsInitialized())
			return;

		DV_EVENT_FIRE(eDvEventType::EndRender);

		// vsync 여부를 멤버로 가져야 할 듯
		// orho는 screenParam으로 전부 가진다.
		m_pSwapChain->Present(1, 0);
	}

	// 윈도우로부터 이벤트를 받아야 한다...
	void DvGraphics::OnResizeWindow()
	{
		// 크기를 전달받거나, Window로부터 얻어야 한다.
		int width = 0;
		int height = 0;

		// backbuffer rendertarget을 새로 만든다.
		updateSwapChain(width, height);

		// render targets들 + viewport 도 새로 만들어야 한다.
		// 그런데 orho는 reset만 한다. 아마 다른 곳에서 새로 만드나 보다.
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
	bool DvGraphics::updateSwapChain(int width, int height)
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

		RECT rt;
		::GetWindowRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Window Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		::GetClientRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Client Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		return true;
	}
}