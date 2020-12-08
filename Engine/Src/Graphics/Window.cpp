#include "DivePch.h"
#include "Window.h"


namespace Dive
{
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}

	Window::Window()
		: m_bFullScreen(false)
	{
		m_title = L"no name";
	}

	bool Dive::Window::Create(int width, int height)
	{
		m_hInstance = static_cast<HINSTANCE>(::GetModuleHandle(NULL));
		m_width		= width;
		m_height	= height;

		std::wstring className{ L"WndClass" };

		WNDCLASSEX wndClass;
		wndClass.cbSize			= sizeof(wndClass);
		wndClass.cbClsExtra		= 0;
		wndClass.cbWndExtra		= 0;
		wndClass.style			= 0;
		wndClass.hInstance		= m_hInstance;
		wndClass.lpfnWndProc	= WndProc;
		wndClass.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
		wndClass.hCursor		= LoadCursor(m_hInstance, IDC_ARROW);
		wndClass.hIcon			= LoadIcon(m_hInstance, IDI_APPLICATION);
		wndClass.hIconSm		= LoadIcon(m_hInstance, IDI_APPLICATION);
		wndClass.lpszClassName	= className.c_str();
		wndClass.lpszMenuName	= nullptr;

		if (!RegisterClassEx(&wndClass))
		{
			MessageBox(nullptr, L"Fail to register window class!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
			return false;
		}

		m_hWnd = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			className.c_str(),
			m_title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			m_width, m_height,
			nullptr, nullptr, m_hInstance, nullptr);

		if (!m_hWnd)
		{
			MessageBox(nullptr, L"Fail to create window!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
			return false;
		}

		return true;
	}

	void Window::Destroy()
	{
		DestroyWindow(m_hWnd);
	}

	bool Window::Run()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));

		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return msg.message != WM_QUIT;
	}

	void Window::Show()
	{
		ShowWindow(m_hWnd, SW_SHOW);
		UpdateWindow(m_hWnd);
		SetFocus(m_hWnd);
	}

	int Window::GetClientRectWidth()
	{
		RECT rt;
		GetClientRect(m_hWnd, &rt);

		return rt.right - rt.left;
	}
	
	int Window::GetClientRectHeight()
	{
		RECT rt;
		GetClientRect(m_hWnd, &rt);

		return rt.bottom - rt.top;
	}
}