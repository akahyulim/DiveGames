#include "Core/Window.h"
#include "Utilities/StringUtils.h"
#include <assert.h>
#include <backends/imgui_impl_win32.h>

// 1. ImGui Win32 백엔드 핸들러 선언 (보통 .cpp 상단이나 헤더에 위치)
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Dive
{
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) // UINT32 대신 표준 Win32 타입인 UINT로 맞추는 것이 좋습니다
	{
		// [핵심] 2. 윈도우 메시지가 오자마자 ImGui가 먼저 처리할 기회를 줍니다.
		// 마우스 클릭, 드래그, 키 입력 등을 ImGui 창 위에서 할 때 게임으로 입력이 새어나가지 않게 막아줍니다.
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		{
			return true; // ImGui가 이 메시지를 소비(처리)했다면 그대로 반환
		}

		LRESULT result = 0ll;

		LONG_PTR ptr = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
		Window* window = reinterpret_cast<Window*>(ptr);

		WindowEventData data{};
		data.handle = hWnd;
		data.msg = static_cast<uint32_t>(msg);
		data.wParam = static_cast<uint64_t>(wParam);
		data.lParam = static_cast<uint64_t>(lParam);
		data.width = window ? static_cast<float>(window->GetWidth()) : 0.0f;
		data.height = window ? static_cast<float>(window->GetHeight()) : 0.0f;

		if (msg == WM_CLOSE || msg == WM_DESTROY)
		{
			::PostQuitMessage(0);
			return 0;
		}
		else if (msg == WM_DISPLAYCHANGE || msg == WM_SIZE)
		{
			data.width = static_cast<float>(lParam & 0xffff);
			data.height = static_cast<float>((lParam >> 16) & 0xffff);
		}
		else
			result = ::DefWindowProc(hWnd, msg, wParam, lParam);

		if (window)
			window->broadcastEvents(data);

		return result;
	}
	/*
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = 0ll;

		LONG_PTR ptr = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
		Window* window = reinterpret_cast<Window*>(ptr);

		WindowEventData data{};
		data.handle = hWnd;
		data.msg = static_cast<uint32_t>(msg);
		data.wParam = static_cast<uint64_t>(wParam);
		data.lParam = static_cast<uint64_t>(lParam);
		data.width = window ? static_cast<float>(window->GetWidth()) : 0.0f;
		data.height = window ? static_cast<float>(window->GetHeight()) : 0.0f;

		if (msg == WM_CLOSE || msg == WM_DESTROY)
		{
			::PostQuitMessage(0);
			return 0;
		}
		else if (msg == WM_DISPLAYCHANGE || msg == WM_SIZE)
		{
			data.width = static_cast<float>(lParam & 0xffff);
			data.height = static_cast<float>((lParam >> 16) & 0xffff);
		}
		else
			result = ::DefWindowProc(hWnd, msg, wParam, lParam);

		if (window)
			window->broadcastEvents(data);

		return result;
	}
	*/
	Window::Window(const WindowInit& init)
	{
		HINSTANCE hInstance = ::GetModuleHandle(nullptr);

		LPCWSTR title = init.title;
		const int width = init.width;
		const int height = init.height;
		LPCWSTR className = L"WindowClass";

		WNDCLASSEX wc{};
		wc.style = 0;
		wc.hInstance = hInstance;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)::GetStockObject(GRAY_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = className;
		wc.cbSize = sizeof(WNDCLASSEX);

		if (!::RegisterClassEx(&wc))
		{
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
			CW_USEDEFAULT,CW_USEDEFAULT,
			rt.right - rt.left, 
			rt.bottom - rt.top,
			NULL, NULL, hInstance, NULL
		);

		if (!m_hWnd)
		{
			::MessageBox(nullptr, L"윈도우 생성 실패", L"Fatal Error!", MB_ICONEXCLAMATION | MB_OK);
			return;
		}

		::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		::ShowWindow(m_hWnd, init.maximize ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL);
		::UpdateWindow(m_hWnd);
		::SetFocus(m_hWnd);
	}

	Window::~Window()
	{
		if(m_hWnd)
			::DestroyWindow(m_hWnd);
	}

	bool Window::Run()
	{
		MSG msg{};
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
			
			if (msg.message == WM_QUIT)
				return false;
		}

		return true;
	}

	void Window::SetMessageCallback(LONG_PTR callBack) const
	{
		if (!::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, callBack))
		{
			::MessageBox(nullptr, L"윈도우 콜백 교체 실패", L"Fatal Error!", MB_ICONEXCLAMATION | MB_OK);
			return;
		}
	}

	bool Window::Resize(uint32_t width, uint32_t height) const
	{
		assert(m_hWnd);

		int posX = (::GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		int posY = (::GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		if (!::SetWindowPos(m_hWnd, NULL, posX, posY, width, height, SWP_NOZORDER | SWP_DRAWFRAME))
		{
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
			::SetWindowLong(m_hWnd, GWL_STYLE, WS_POPUP);
			::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
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

	void Window::Minimize() const
	{
		assert(m_hWnd != 0);
		if (!::IsIconic(m_hWnd))
			::ShowWindow(m_hWnd, SW_MINIMIZE);
	}

	void Window::Maximize() const
	{
		assert(m_hWnd != 0);
		if (!::IsZoomed(m_hWnd))
			::ShowWindow(m_hWnd, SW_MAXIMIZE);
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