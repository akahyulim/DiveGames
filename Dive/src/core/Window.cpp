#include "stdafx.h"
#include "Window.h"
#include "CoreDefs.h"

namespace Dive
{
	constexpr LPCWCHAR WND_CLASS_NAME = L"DIVE_WINDOW";
	constexpr int DEFAULT_WIDTH = 1280;
	constexpr int DEFAULT_HEIGHT = 720;

	HINSTANCE Window::s_hInstance = nullptr;
	HWND Window::s_hWnd = nullptr;
	std::wstring Window::s_Title = L"DIVE";

	void Window::Initialize()
	{
		s_hInstance = ::GetModuleHandle(nullptr);

		WNDCLASSEX wc{};
		wc.style = 0;
		wc.hInstance = s_hInstance;
		wc.lpfnWndProc = ::DefWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = WND_CLASS_NAME;
		wc.cbSize = sizeof(WNDCLASSEX);

		if (!::RegisterClassEx(&wc)) 
		{
			DV_LOG(Window, err, "윈도우 클래스 등록 실패");
			return;
		}

		DWORD style = WS_OVERLAPPEDWINDOW;

		int posX = (GetSystemMetrics(SM_CXSCREEN) - DEFAULT_WIDTH) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - DEFAULT_HEIGHT) / 2;

		s_hWnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			WND_CLASS_NAME,
			s_Title.c_str(),
			style,
			posX > 0 ? posX : 0,
			posY > 0 ? posY : 0,
			DEFAULT_WIDTH, DEFAULT_HEIGHT,
			NULL, NULL,
			s_hInstance,
			NULL);

		if (!s_hWnd)
		{
			DV_LOG(Window, critical, "윈도우 생성 실패");
			return;
		}

		Show();
	}

	void Window::Shutdown()
	{
		::ShowCursor(TRUE);

		::DestroyWindow(s_hWnd);
		s_hWnd = nullptr;

		::UnregisterClass(WND_CLASS_NAME, s_hInstance);
		s_hInstance = nullptr;
	}

	bool Window::Tick()
	{
		MSG msg{};
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}

		return msg.message != WM_QUIT;
	}

	void Window::SetMessageCallback(LONG_PTR callBack)
	{
		if (!SetWindowLongPtr(s_hWnd, GWLP_WNDPROC, callBack))
			DV_LOG(Window, err, "프로시져 함수 변경에 실패하였습니다.");
	}

	bool Window::Resize(const uint32_t width, const uint32_t height)
	{
		DV_ASSERT(Window, s_hWnd);

		int posX = (::GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		int posY = (::GetSystemMetrics(SM_CYSCREEN) - height) / 2;
		
		if (!::SetWindowPos(s_hWnd, NULL, posX, posY, width, height, SWP_NOZORDER | SWP_DRAWFRAME))
		{
			DV_LOG(Window, err, "윈도우 크기 변경 실패");
			return false;
		}

		return true;
	}

	uint32_t Window::GetWidth()
	{
		DV_ASSERT(Window, s_hWnd);

		RECT rt{};
		::GetClientRect(s_hWnd, &rt);

		return static_cast<uint32_t>(rt.right - rt.left);
	}

	uint32_t Window::GetHeight()
	{
		DV_ASSERT(Window, s_hWnd);

		RECT rt{};
		::GetClientRect(s_hWnd, &rt);

		return static_cast<uint32_t>(rt.bottom - rt.top);
	}

	void Window::Show()
	{
		DV_ASSERT(Window, s_hWnd);
		::ShowWindow(s_hWnd, SW_SHOW);
		::SetForegroundWindow(s_hWnd);
		::SetFocus(s_hWnd);
	}

	void Window::Hide()
	{
		DV_ASSERT(Window, s_hWnd);
		::ShowWindow(s_hWnd, SW_HIDE);
	}

	void Window::Close()
	{
		DV_ASSERT(Window, s_hWnd);
		::PostQuitMessage(0);
	}

	bool Window::IsFullScreen()
	{
		DV_ASSERT(Window, s_hWnd);

		if (!(GetWindowLong(s_hWnd, GWL_STYLE) & WS_POPUP))
			return false;

		RECT rt{};
		::GetClientRect(s_hWnd, &rt);

		return (::GetSystemMetrics(SM_CXSCREEN) == static_cast<int>(rt.right - rt.left)) &&
			(::GetSystemMetrics(SM_CYSCREEN) == static_cast<int>(rt.bottom - rt.top));
	}
	
	void Window::FullScreen()
	{
		DV_ASSERT(Window, s_hWnd);

		if (!IsFullScreen())
		{
			SetWindowLong(s_hWnd, GWL_STYLE, WS_POPUP);
			SetWindowPos(s_hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
	}

	void Window::SetWindowed(bool windowed)
	{
		DV_ASSERT(Window, s_hWnd);

		LONG currentStyle = GetWindowLong(s_hWnd, GWL_STYLE);

		if (windowed)
		{
			LONG newStyle = WS_POPUP;
			if (currentStyle != newStyle)
			{
				SetWindowLong(s_hWnd, GWL_STYLE, newStyle);
				SetWindowPos(s_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		else
		{
			LONG newStyle = WS_OVERLAPPEDWINDOW;
			if (currentStyle != newStyle)
			{
				SetWindowLong(s_hWnd, GWL_STYLE, newStyle);
				SetWindowPos(s_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
	}
	
	void Window::Minimize()
	{
		DV_ASSERT(Window, s_hWnd);

		if (!::IsIconic(s_hWnd))
			::ShowWindow(s_hWnd, SW_MINIMIZE);
	}
	
	void Window::Maximize()
	{
		DV_ASSERT(Window, s_hWnd);

		if (!::IsZoomed(s_hWnd))
			::ShowWindow(s_hWnd, SW_MAXIMIZE);
	}

	void Window::Restore()
	{
		DV_ASSERT(Window, s_hWnd);

		if (::IsZoomed(s_hWnd))
			::ShowWindow(s_hWnd, SW_RESTORE);
	}

	bool Window::IsMaximize()
	{
		DV_ASSERT(Window, s_hWnd);
		
		return ::IsZoomed(s_hWnd);
	}

	void Window::SetTitle(const std::wstring& title)
	{
		DV_ASSERT(Window, s_hWnd);

		if (s_Title != title)
		{
			::SetWindowText(s_hWnd, title.c_str());
			s_Title = title;
		}
	}
}