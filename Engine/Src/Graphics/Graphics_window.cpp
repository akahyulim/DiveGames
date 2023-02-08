#include "divepch.h"
#include "Graphics.h"
#include "GraphicsEvents.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "InputLayout.h"
#include "Shader.h"
#include "Renderer/Model.h"
#include "Renderer/Material.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Core/EventSystem.h"
#include "Resource/ResourceCache.h"
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
		m_hInstance(nullptr),
		m_WindowHandle(nullptr),
		m_WindowTitle(L"Dive"),
		m_WindowPosition(-1, -1),
		m_WindowStyles(0),
		m_WindowFlags(0),
		m_Width(800),
		m_Height(600),
		m_pDevice(nullptr),
		m_pDeviceContext(nullptr),
		m_pSwapChain(nullptr),
		m_pDefaultRenderTargetView(nullptr),
		m_pDefaultDepthStencilTexture(nullptr),
		m_pDefaultDepthStencilView(nullptr),
		m_PrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED),
		m_pDepthStencil(nullptr),
		m_pCurDepthStencilView(nullptr),
		m_pVertexBuffer(nullptr),
		m_pIndexBuffer(nullptr),
		m_pLastShader(nullptr),
		m_InputLayoutHash(0),
		m_FirstDirtyTexture(0),
		m_LastDirtyTexture(0),
		m_bTextureDirty(true),
		m_bVertexTypeDirty(true),
		m_bRenderTargetsDirty(true),
		m_pCurShaderProgram(nullptr),
		m_pVertexShaderVariation(nullptr),
		m_pPixelShaderVariation(nullptr)
	{
		s_pGraphics = this;

		// 쉐이더 경로 및 파일 포멧 설정
		// 이외에도 기타 설정이 있다.
		
		for (uint32_t i = 0; i < MAX_RENDERTARGETS; ++i)
		{
			m_pRenderTargets[i] = nullptr;
			m_pCurRenderTargetViews[i] = nullptr;
		}

		for (uint32_t i = 0; i < 16; ++i)
		{
			m_pTextures[i] = nullptr;
			m_pShaderResourceViews[i] = nullptr;
			m_pSamplers[i] = nullptr;
		}

		for (uint32_t i = 0; i < 7; ++i)
		{
			m_pCurVertexShaderConstantBuffers[i] = nullptr;
			m_pCurPixelShaderConstantBuffers[i] = nullptr;
		}
	}

	Graphics::~Graphics()
	{
		Destroy();

		DV_LOG_ENGINE_TRACE("Graphics 소멸 완료");
	}

	bool Graphics::SetMode(int32_t width, int32_t height, bool bFullscreen, bool bBorderless, bool bResizable, bool bVSync,
		bool tripleBuffer, int32_t multiSample, int32_t refreshRate)
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

		uint32_t flags = 0;
		flags |= bFullscreen ? WINDOW_FULLSCREEN : 0;
		flags |= bBorderless ? WINDOW_BORDERLESS : 0;
		flags |= bResizable ? WINDOW_RESIZABLE : 0;

		// 현재 윈도우 생성 후 바로 최소화하면 먹통이 되는 버그가 있다.
		// 아무래도 크기를 저장하지 못했기 때문인 듯 하다?
		// => 수정한 부분이 없는데 문제가 사라졌다...
		if (!WindowCreate(width, height, flags))
			return false;

		if (!CreateDeviceAndSwapChain())
			return false;

		if (!UpdateSwapChain())
			return false;

		// 위치가 애매허다?
		RegisterGraphicsObject(m_pContext);

		// clear
		// present

		// OnScreenModeChanged

		// 이건 내가 추가.
		ShowWindow();

		return true;
	}

	void Graphics::Destroy()
	{
		// input Layouts
		for (auto it : m_InputLayouts)
			DV_DELETE(it.second);
		m_InputLayouts.clear();

		m_pCurShaderProgram = nullptr;
		for (auto pShaderProgram : m_ShaderPrograms)
			DV_DELETE(pShaderProgram.second);
		m_ShaderPrograms.clear();

		// constant buffers
		for (auto pConstantBuffer : m_AllConstantBuffers)
			DV_DELETE(pConstantBuffer.second);
		m_AllConstantBuffers.clear();

		// 각종 states

		DV_RELEASE(m_pDefaultDepthStencilView);
		DV_RELEASE(m_pDefaultDepthStencilTexture);
		DV_RELEASE(m_pDefaultRenderTargetView);

		DV_RELEASE(m_pSwapChain);
		DV_RELEASE(m_pDeviceContext);
		DV_RELEASE(m_pDevice);
		
		CloseWindow();

		s_pGraphics = nullptr;
	}

	bool Graphics::IsInitialized() const
	{
		return (m_WindowHandle && m_pDevice && m_pDeviceContext);
	}

	bool Graphics::WindowCreate(int32_t width, int32_t height, uint32_t flags)
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

		m_WindowHandle = CreateWindowEx(
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

		if (!m_WindowHandle)
		{
			DV_LOG_ENGINE_ERROR("Graphics::WindowCreate - 윈도우 생성에 실패하였습니다.");
			return false;
		}

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
				return 0;
			}
		}

		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	void Graphics::CloseWindow()
	{
		if (!m_WindowHandle)
			return;

		// 전체화면에서 스왑체인 릴리즈시 예외 발생
		if (IsFullScreen())
			m_pSwapChain->SetFullscreenState(FALSE, nullptr);

		ShowCursor(true);

		if (IsFullScreen())
			::ChangeDisplaySettings(nullptr, 0);

		::DestroyWindow(m_WindowHandle);
		m_WindowHandle = NULL;

		::UnregisterClassW(WND_CLASS_NAME, m_hInstance);
		m_hInstance = NULL;

		DV_LOG_ENGINE_TRACE("윈도우를 제거하였습니다.");
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
		if (m_WindowHandle)
			::SetWindowText(m_WindowHandle, title.c_str());
		
		m_WindowTitle = title;
	}

	void Graphics::GetWindowPosition(int32_t& outX, int32_t& outY) const
	{
		if (m_WindowHandle)
		{
			outX = m_WindowPosition.x;
			outY = m_WindowPosition.y;
		}
	}

	void Graphics::SetWindowPosition(int32_t x, int32_t y)
	{
		if (!IsInitialized())
			return;

		// 전체화면이라면 변경 불가.
		if (m_WindowFlags & WINDOW_FULLSCREEN)
			return;

		RECT rt{ 0, 0, m_Width, m_Height };
		::AdjustWindowRect(&rt, m_WindowStyles, FALSE);

		::SetWindowPos(
			m_WindowHandle, 
			NULL, 
			x, 
			y,
			rt.right - rt.left,
			rt.bottom - rt.top,
			SWP_SHOWWINDOW);

		m_WindowPosition.x = x;
		m_WindowPosition.y = y;

		::GetWindowRect(m_WindowHandle, &rt);
		DV_LOG_ENGINE_DEBUG("Window Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		::GetClientRect(m_WindowHandle, &rt);
		DV_LOG_ENGINE_DEBUG("Client Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);
	}

	// 앱에서 윈도우 크기 및 해상도를 변경하는 함수
	// 일단 이 곳에서 스왑체인을 리사이즈 하면 WM_SIZE가 발생하고
	// 이를 통해 OnResizeWindow()를 통해 백퍼버와 렌더타켓을 갱신한다.
	void Graphics::ResizeWindow(int32_t width, int32_t height)
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
			DV_LOG_ENGINE_ERROR("Graphics::ResizeWindow - 스왑체인 타겟 리사이즈에 실패하였습니다.");
			return;
		}
	}

	void Graphics::ShowWindow()
	{
		if (m_WindowHandle)
		{
			::ShowWindow(m_WindowHandle, SW_SHOW);
			::SetFocus(m_WindowHandle);
		}
	}

	void Graphics::HideWindow()
	{
		if (m_WindowHandle)
		{
			::ShowWindow(m_WindowHandle, SW_HIDE);
		}
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
	}

	void Graphics::SetBorderlessWindow(bool bBorderelss)
	{
		/*
		* if (!m_WindowHandle)
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
			::SetWindowLongPtr(m_WindowHandle, GWL_STYLE, m_Styles);
			::SetWindowPos(m_WindowHandle, NULL, m_Position.x, m_Position.y, rt.right - rt.left, rt.bottom - rt.top, SWP_SHOWWINDOW);
		}

		RECT rt;
		::GetWindowRect(m_WindowHandle, &rt);
		DV_LOG_ENGINE_DEBUG("Window Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		::GetClientRect(m_WindowHandle, &rt);
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
}