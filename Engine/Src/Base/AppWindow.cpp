#include "divepch.h"
#include "AppWindow.h"
#include "DiveCore.h"
#include "Log.h"

namespace Dive
{
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		}

		return DefWindowProcW(hWnd, msg, wParam, lParam);
	}
	
	AppWindow::~AppWindow()
	{
		Destroy();
	}

	void AppWindow::Create()
	{
		if (m_hInstance == 0)
			m_hInstance = GetModuleHandle(NULL);

		std::wstring windowName;
		windowName.assign(m_Title.begin(), m_Title.end());

		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = 0;
		wcex.lpfnWndProc	= WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= m_hInstance;
		wcex.hIcon			= LoadIcon(nullptr, IDI_APPLICATION);
		wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName	= nullptr;
		wcex.lpszClassName	= windowName.c_str();
		wcex.hIconSm		= LoadIcon(nullptr, IDI_APPLICATION);

		DV_ASSERT(RegisterClassEx(&wcex));

		int posX = (GetSystemMetrics(SM_CXSCREEN) - (int)m_Width) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - (int)m_Height) / 2;

		auto hWnd = CreateWindowW(windowName.c_str(), windowName.c_str(), WS_OVERLAPPEDWINDOW,
			posX, posY, m_Width, m_Height, nullptr, nullptr, m_hInstance, nullptr);

		DV_ASSERT(hWnd != NULL);

		ShowWindow(hWnd, SW_SHOW);
		SetForegroundWindow(hWnd);
		SetFocus(hWnd);
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
}