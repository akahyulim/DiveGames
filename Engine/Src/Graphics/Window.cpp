#include "DivePch.h"
#include "Window.h"
#include "Graphics/Graphics.h"

namespace Dive
{
	extern Graphics* g_graphics;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

		default:
			return g_graphics->MessageHandler(hWnd, msg, wParam, lParam);
		}
	}

	Window::Window()
		: m_style(0)
	{
		m_title = L"no name";
		m_baseWndProc = WndProc;
	}

	bool Dive::Window::Create(int width, int height, bool fullScreen, bool windowed)
	{
		m_hInstance = static_cast<HINSTANCE>(::GetModuleHandle(NULL));
		std::wstring className{ L"WndClass" };

		WNDCLASSEX wndClass;
		wndClass.cbSize			= sizeof(wndClass);
		wndClass.cbClsExtra		= 0;
		wndClass.cbWndExtra		= 0;
		wndClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //0;
		wndClass.hInstance		= m_hInstance;
		wndClass.lpfnWndProc	= m_baseWndProc;
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

		if (fullScreen)
		{
			width = GetSystemMetrics(SM_CXSCREEN);
			height = GetSystemMetrics(SM_CYSCREEN);

			DEVMODE devMode;
			ZeroMemory(&devMode, sizeof(devMode));
			devMode.dmSize = sizeof(devMode);
			devMode.dmBitsPerPel = 32;
			devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			devMode.dmPelsWidth = static_cast<unsigned long>(width);
			devMode.dmPelsHeight = static_cast<unsigned long>(height);

			ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);

			m_positionX = 0;
			m_positionY = 0;
		}
		else
		{
			if (width <= 0 || height <= 0)
			{
				width = 1024;
				height = 768;
			}

			m_positionX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
			m_positionY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
		}

		m_width = width;
		m_height = height;
		m_style = (windowed) ? WS_OVERLAPPEDWINDOW : WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;

		m_hWnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			className.c_str(),
			m_title.c_str(),
			m_style,
			m_positionX,
			m_positionY,
			m_width,
			m_height,
			nullptr, nullptr,
			m_hInstance,
			nullptr);

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

	void Window::Show(bool show)
	{
		ShowWindow(m_hWnd, show ? SW_SHOW : SW_HIDE);
		UpdateWindow(m_hWnd);
		SetFocus(m_hWnd);
	}

	bool Window::ChangeWndProc(LONG newWndProc)
	{
		m_baseWndProc = (WNDPROC)SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, newWndProc);
		return false;
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

	bool Window::SetTitle(std::wstring title)
	{
		return SetWindowText(m_hWnd, title.c_str());
	}
}