#include "Window.h"
#include <assert.h>

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
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		default:
			return DefWindowProcW(hWnd, msg, wParam, lParam);
		}
	}

	Window::Window(HINSTANCE hInstance, const WindowData& data)
		: m_Data(data)
	{
		m_hWnd = 0;
		m_hInstance = hInstance;

		LPCWSTR szWindowClass = L"EditorWindowClass";

		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = 0;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = m_hInstance;
		wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

		assert(RegisterClassEx(&wcex));
		
		int posX = (GetSystemMetrics(SM_CXSCREEN) - (int)m_Data.Width) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - (int)m_Data.Height) / 2;

		m_hWnd = CreateWindowW(szWindowClass, m_Data.Title.c_str(), WS_OVERLAPPEDWINDOW,
			posX, posY, m_Data.Width, m_Data.Height, nullptr, nullptr, m_hInstance, nullptr);

		assert(m_hWnd != NULL);

		ShowWindow(m_hWnd, SW_SHOW);
		SetForegroundWindow(m_hWnd);
		SetFocus(m_hWnd);
	}

	Window::~Window()
	{
		if (m_hWnd)
			Destroy();
	}
	
	bool Window::Run()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);

			if (msg.message == WM_QUIT)
				return false;
		}

		return true;
	}

	void Window::Destroy()
	{
		DestroyWindow(m_hWnd);
	}
}