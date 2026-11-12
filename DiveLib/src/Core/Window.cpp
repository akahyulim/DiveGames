#include "Core/Window.h"
#include "Core/LogManager.h"
#include "Utilities/StringUtils.h"
#include <assert.h>

namespace Dive
{
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		Window* window = nullptr;

		if (msg == WM_NCCREATE)
		{
			// 창이 생성되는 최초 순간에 this 포인터를 받아와서 GWLP_USERDATA에 저장
			auto* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
			window = reinterpret_cast<Window*>(createStruct->lpCreateParams);
			window->m_hWnd = hWnd;

			::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		}
		else
		{
			// 이후부터는 저장해둔 포인터를 가져옴
			window = reinterpret_cast<Window*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		}

		// 창 객체가 존재한다면 멤버 함수로 메시지 토스
		if (window)
		{
			return window->HandleMessage(hWnd, msg, wParam, lParam);
		}

		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	Window::Window(int width, int height, const wchar_t* title)
	{
		DV_LOG(Window, info, "Initializing Win32 Window (Width: {}, Height: {})...", width, height);

		HINSTANCE hInstance = ::GetModuleHandle(nullptr);

		LPCWSTR className = L"WindowClass";

		WNDCLASSEX wc{};
		wc.style = 0;
		wc.hInstance = hInstance;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)::GetStockObject(GRAY_BRUSH);
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = className;
		wc.cbSize = sizeof(WNDCLASSEX);

		if (!::RegisterClassEx(&wc))
		{
			DV_LOG(Window, err, "Failed to register window class.");
			::MessageBoxA(nullptr, "윈도우 클래스 등록 실패", "Fatal Error!", MB_ICONEXCLAMATION | MB_OK);
			return;
		}

		RECT rt = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
		::AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, FALSE);

		m_hWnd = ::CreateWindowEx(
			0,
			className,
			title,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			rt.right - rt.left,
			rt.bottom - rt.top,
			nullptr, nullptr, hInstance,
			this
		);

		if (!m_hWnd)
		{
			DV_LOG(Window, err, "Failed to create window handle.");
			::MessageBox(nullptr, L"윈도우 생성 실패", L"Fatal Error!", MB_ICONEXCLAMATION | MB_OK);
			return;
		}

		::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		::ShowWindow(m_hWnd, SW_SHOWNORMAL);
		::UpdateWindow(m_hWnd);
		::SetFocus(m_hWnd);

		DV_LOG(Window, info, "Win32 Window initialized successfully. (HWND: {})", static_cast<void*>(m_hWnd));
	}

	Window::~Window()
	{
		if (m_hWnd)
		{
			DV_LOG(Window, info, "Destroying Win32 Window (HWND: {})...", static_cast<void*>(m_hWnd));
			::DestroyWindow(m_hWnd);
			m_hWnd = nullptr;
			DV_LOG(Window, info, "Win32 Window destroyed successfully.");
		}
	}

	LRESULT Window::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		WindowEventData data{};
		data.handle = hWnd;
		data.msg = static_cast<uint32_t>(msg);
		data.wParam = static_cast<uint64_t>(wParam);
		data.lParam = static_cast<uint64_t>(lParam);
		data.width = static_cast<float>(GetWidth());
		data.height = static_cast<float>(GetHeight());

		LRESULT result = 0;

		switch (msg)
		{
		case WM_CLOSE:
		case WM_DESTROY:
			m_shouldClose = true;
			::PostQuitMessage(0);
			result = 0;
			break;

		case WM_SIZE:
			data.width = static_cast<float>(LOWORD(lParam));
			data.height = static_cast<float>(HIWORD(lParam));
			// 필요하다면 여기서 내부 크기 멤버 갱신 처리
			result = ::DefWindowProc(hWnd, msg, wParam, lParam);
			break;

		default:
			result = ::DefWindowProc(hWnd, msg, wParam, lParam);
			break;
		}

		// 이벤트 브로드캐스트
		broadcastEvents(data);

		return result;
	}

	void Window::PollEvents()
	{
		MSG msg{};
		while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				m_shouldClose = true;

			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}
	}

	void Window::SetMessageCallback(LONG_PTR callBack) const
	{
		if (!::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, callBack))
		{
			DV_LOG(Window, err, "Failed to set window message callback (WndProc).");
			::MessageBox(nullptr, L"윈도우 콜백 교체 실패", L"Fatal Error!", MB_ICONEXCLAMATION | MB_OK);
			return;
		}
		DV_LOG(Window, info, "Window message callback successfully updated.");
	}

	bool Window::Resize(uint32_t width, uint32_t height) const
	{
		assert(m_hWnd);
		DV_LOG(Window, info, "Resizing window to {}x{}...", width, height);

		int posX = (::GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		int posY = (::GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		if (!::SetWindowPos(m_hWnd, nullptr, posX, posY, width, height, SWP_NOZORDER | SWP_DRAWFRAME))
		{
			DV_LOG(Window, err, "Failed to resize window position/size.");
			::MessageBox(nullptr, L"윈도우 크기 변경 실패", L"Fatal Error!", MB_ICONEXCLAMATION | MB_OK);
			return false;
		}

		return true;
	}

	uint32_t Window::GetWidth() const
	{
		assert(m_hWnd != 0);

		RECT rt{};
		::GetClientRect(m_hWnd, &rt);

		return static_cast<uint32_t>(rt.right - rt.left);
	}

	uint32_t Window::GetHeight() const
	{
		assert(m_hWnd != 0);

		RECT rt{};
		::GetClientRect(m_hWnd, &rt);

		return static_cast<uint32_t>(rt.bottom - rt.top);
	}

	bool Window::IsActive() const
	{
		return ::GetForegroundWindow() == m_hWnd;
	}

	void Window::Show() const
	{
		assert(m_hWnd != 0);
		::ShowWindow(m_hWnd, SW_SHOW);
		::SetForegroundWindow(m_hWnd);
		::SetFocus(m_hWnd);
	}

	void Window::Hide() const
	{
		assert(m_hWnd != 0);
		::ShowWindow(m_hWnd, SW_HIDE);
	}

	void Window::Close() const
	{
		assert(m_hWnd != 0);
		::PostQuitMessage(0);
	}

	bool Window::IsWindowed() const
	{
		assert(m_hWnd != 0);

		if ((GetWindowLong(m_hWnd, GWL_STYLE) & WS_POPUP))
			return false;

		RECT rt{};
		::GetClientRect(m_hWnd, &rt);

		return (::GetSystemMetrics(SM_CXSCREEN) != static_cast<int>(rt.right - rt.left)) ||
			(::GetSystemMetrics(SM_CYSCREEN) != static_cast<int>(rt.bottom - rt.top));
	}

	void Window::FullScreen() const
	{
		assert(m_hWnd != 0);

		if (IsWindowed())
		{
			DV_LOG(Window, info, "Switching window to FullScreen mode.");
			::SetWindowLong(m_hWnd, GWL_STYLE, WS_POPUP);
			::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
	}

	void Window::Minimize() const
	{
		assert(m_hWnd != 0);
		if (!::IsIconic(m_hWnd))
		{
			DV_LOG(Window, info, "Minimizing window.");
			::ShowWindow(m_hWnd, SW_MINIMIZE);
		}
	}

	void Window::Maximize() const
	{
		assert(m_hWnd != 0);
		if (!::IsZoomed(m_hWnd))
		{
			DV_LOG(Window, info, "Maximizing window.");
			::ShowWindow(m_hWnd, SW_MAXIMIZE);
		}
	}

	void Window::SetWindowed(bool windowed) const
	{
		assert(m_hWnd != 0);

		LONG currentStyle = GetWindowLong(m_hWnd, GWL_STYLE);

		if (windowed)
		{
			LONG newStyle = WS_POPUP;
			if (currentStyle != newStyle)
			{
				::SetWindowLong(m_hWnd, GWL_STYLE, newStyle);
				::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		else
		{
			LONG newStyle = WS_OVERLAPPEDWINDOW;
			if (currentStyle != newStyle)
			{
				::SetWindowLong(m_hWnd, GWL_STYLE, newStyle);
				::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
	}

	void Window::Restore() const
	{
		assert(m_hWnd != 0);
		if (::IsZoomed(m_hWnd))
			::ShowWindow(m_hWnd, SW_RESTORE);
	}

	bool Window::IsMaximize() const
	{
		assert(m_hWnd != 0);
		return ::IsZoomed(m_hWnd);
	}

	void Window::SetTitle(const std::string& title) const
	{
		auto newTitle = StringUtils::ToWString(title);
		::SetWindowText(m_hWnd, newTitle.c_str());
	}

	void Window::broadcastEvents(const WindowEventData& data)
	{
		m_windowEvent.Broadcast(data);

		switch (data.msg)
		{
		case WM_ENTERSIZEMOVE:
			m_isResizing = true;
			break;
		case WM_EXITSIZEMOVE:
			m_isResizing = false;
			m_resizedEvent.Broadcast(static_cast<uint32_t>(data.width), static_cast<uint32_t>(data.height));
			break;
		case WM_SIZE:
			if(!m_isResizing)
				m_resizedEvent.Broadcast(static_cast<uint32_t>(data.width), static_cast<uint32_t>(data.height));
			break;
		}
	}
}