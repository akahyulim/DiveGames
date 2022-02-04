#include "divepch.h"
#include "AppWindow.h"
#include "DiveCore.h"

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

	AppWindow::AppWindow(HINSTANCE hInstance, const std::string& title)
	{
		// load ini
		unsigned int width = 800;
		unsigned int height = 600;

		std::wstring windowName;
		windowName.assign(title.begin(), title.end());

		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style			= 0;
		wcex.lpfnWndProc	= WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= LoadIcon(nullptr, IDI_APPLICATION);
		wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName	= nullptr;
		wcex.lpszClassName	= windowName.c_str();
		wcex.hIconSm		= LoadIcon(nullptr, IDI_APPLICATION);

		DV_ASSERT(RegisterClassEx(&wcex));

		int posX = (GetSystemMetrics(SM_CXSCREEN) - (int)width) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - (int)height) / 2;

		auto hWnd = CreateWindowW(windowName.c_str(), windowName.c_str(), WS_OVERLAPPEDWINDOW,
			posX, posY, width, height, nullptr, nullptr, hInstance, nullptr);

		DV_ASSERT(hWnd != NULL);

		m_hInstance		= hInstance;
		m_hWnd			= hWnd;
		m_Title			= title;
		m_Width			= width;
		m_Height		= height;

		ShowWindow(hWnd, SW_SHOW);
		SetForegroundWindow(hWnd);
		SetFocus(hWnd);
	}

	AppWindow::~AppWindow()
	{
	}
	
	bool AppWindow::Run()
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

	void AppWindow::Destroy()
	{
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;

		std::wstring windowName;
		windowName.assign(m_Title.begin(), m_Title.end());

		UnregisterClassW(windowName.c_str(), m_hInstance);
		m_hInstance = nullptr;
	}
}