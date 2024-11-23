#include "divepch.h"
#include "Window.h"
#include "Log.h"
#include "Engine.h"

namespace Dive
{
	constexpr LPCWSTR WND_CLASS_NAME = L"DiveWindow";
	constexpr uint32_t WND_WIDTH = 800;
	constexpr uint32_t WND_HEIGHT = 600;

	HINSTANCE Window::s_hInstance = nullptr;
	HWND Window::s_hWnd = nullptr;
	std::wstring Window::s_Title = L"Dive";
	bool Window::s_bWindowed = true;

	LRESULT CALLBACK DvWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = NULL;

		switch (msg)
		{
		case WM_CLOSE:
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		// 이 프로시져를 Window에서 함수 포인터로 설정할 수 있게 만들고 싶다.
		return GEngine->MessageHandler(hWnd, msg, wParam, lParam);
	}

	bool Window::Initialize()
	{
		s_hInstance = ::GetModuleHandle(nullptr);

		WNDCLASSEX wc{};
		wc.style = 0;
		wc.hInstance = s_hInstance;
		wc.lpfnWndProc = DvWndProc;
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
			return false;
		}

		DWORD style = WS_OVERLAPPEDWINDOW;
		s_bWindowed = true;

		int posX = (GetSystemMetrics(SM_CXSCREEN) - static_cast<int>(WND_WIDTH)) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - static_cast<int>(WND_HEIGHT)) / 2;

		s_hWnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			WND_CLASS_NAME,
			s_Title.c_str(),
			style,
			posX > 0 ? posX : 0,
			posY > 0 ? posY : 0,
			WND_WIDTH, WND_HEIGHT,
			NULL, NULL,
			s_hInstance,
			NULL);

		if (!s_hWnd)
		{
			DV_LOG(Window, critical, "윈도우 생성 실패");
			return false;
		}

		return Resize(WND_WIDTH, WND_HEIGHT);
	}

	void Window::Shutdown()
	{
		::ShowCursor(TRUE);

		::DestroyWindow(s_hWnd);
		s_hWnd = nullptr;

		::UnregisterClass(WND_CLASS_NAME, s_hInstance);

		DV_LOG(Window, trace, "called shutdown");
	}

	bool Window::Run()
	{
		MSG msg{};
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}

		return msg.message != WM_QUIT;
	}

	// width, height를 각각 0으로 전달할 경우 창 or 보더리스 모드만 변경 
	bool Window::Resize(uint32_t width, uint32_t height, bool windowed)
	{
		if (s_hWnd)
		{
			DWORD style = windowed ? WS_OVERLAPPEDWINDOW : WS_POPUP;

			if (s_bWindowed != windowed)
			{
				s_bWindowed = windowed;

				if (!::SetWindowLongPtr(s_hWnd, GWL_STYLE, style))
				{
					DV_LOG(Window, err, "윈도우 스타일 변경 실패");
					return false;
				}
			}

			RECT rt = { 0, 0,
				width != 0 ? static_cast<LONG>(width) : static_cast<LONG>(GetWidth()),
				height != 0 ? static_cast<LONG>(height) : static_cast<LONG>(GetHeight()) };

			if (!::AdjustWindowRect(&rt, style, FALSE))
			{
				DV_LOG(Window, err, "스타일에 맞춘 윈도우 크기 계산 실패");
				return false;
			}

			int newWidth = rt.right - rt.left;
			int newHeight = rt.bottom - rt.top;
			int posX = (::GetSystemMetrics(SM_CXSCREEN) - newWidth) / 2;
			int posY = (::GetSystemMetrics(SM_CYSCREEN) - newHeight) / 2;

			if (!::SetWindowPos(s_hWnd, NULL, posX, posY, newWidth, newHeight, SWP_DRAWFRAME))
			{
				DV_LOG(Window, err, "윈도우 크기 변경 실패");
				return false;
			}

			ShowWindow(SW_SHOW);
		}

		return true;
	}

	DirectX::XMUINT2 Window::GetSize()
	{
		if (!s_hWnd)
			return { 0, 0 };

		RECT rt{};
		::GetClientRect(s_hWnd, &rt);

		return { static_cast<uint32_t>(rt.right - rt.left), static_cast<uint32_t>(rt.bottom - rt.top) };
	}

	uint32_t Window::GetWidth()
	{
		return GetSize().x;
	}

	uint32_t Window::GetHeight()
	{
		return GetSize().y;
	}

	void Window::SetTitle(LPCWSTR title)
	{
		if (title != nullptr)
		{
			s_Title = title;

			if (s_hWnd)
				::SetWindowText(s_hWnd, title);
		}
	}

	void Window::ShowWindow(int nCmdShow)
	{
		::ShowWindow(s_hWnd, nCmdShow);
	}
}