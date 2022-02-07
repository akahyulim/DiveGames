#include "AppWindow.h"
#include "Editor.h"
#include "DiveEngine.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Dive::WindowData data;
	data.hWnd = hWnd;
	data.msg = msg;
	data.wParam = wParam;
	data.lParam = lParam;

	switch (msg)
	{
	case WM_DISPLAYCHANGE:
	{
		data.Width = static_cast<unsigned int>(lParam & 0xffff);
		data.Height = static_cast<unsigned int>((lParam >> 16) & 0xffff);
		break;
	}
	case WM_SIZE:
	{
		data.Width = static_cast<unsigned int>(lParam & 0xffff);
		data.Height = static_cast<unsigned int>((lParam >> 16) & 0xffff);
		break;
	}
	case WM_SYSCOMMAND:
	{
		data.Minimize = wParam == SC_MINIMIZE;
		data.Maximize = wParam == SC_MAXIMIZE;
		break;
	}
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

	// 헐... 생성된건데..
	// 왜 if가 지나치냐...?
	if (Dive::g_pOnMessage)
	{
		Dive::g_pOnMessage(data);
	}

	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

namespace Dive
{
	AppWindow::AppWindow(HINSTANCE hInstance, const EditorData& data)
	{
		if (m_hInstance == 0)
			m_hInstance = GetModuleHandle(NULL);

		m_Title		= data.Title;
		m_Width		= data.Width;
		m_Height	= data.Height;
		m_bMaximize = data.Maximize;

		std::wstring windowName;
		windowName.assign(m_Title.begin(), m_Title.end());

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
		wcex.lpszClassName = windowName.c_str();
		wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

		DV_ASSERT(RegisterClassEx(&wcex));

		int posX = (GetSystemMetrics(SM_CXSCREEN) - (int)m_Width) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - (int)m_Height) / 2;

		auto hWnd = CreateWindowW(windowName.c_str(), windowName.c_str(), WS_OVERLAPPEDWINDOW,
			posX, posY, m_Width, m_Height, nullptr, nullptr, m_hInstance, nullptr);

		DV_ASSERT(hWnd != NULL);

		m_hInstance = hInstance;
		m_hWnd = hWnd;
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

	void AppWindow::Show()
	{
		DV_ASSERT(m_hWnd);

		ShowWindow(m_hWnd, SW_SHOW);
		SetForegroundWindow(m_hWnd);
		SetFocus(m_hWnd);
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