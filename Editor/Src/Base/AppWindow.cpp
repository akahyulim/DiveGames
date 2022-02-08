#include "AppWindow.h"
#include "Editor.h"
#include "DiveEngine.h"
#include "imgui-docking/imgui_impl_win32.h"

// 이게 좀 애매하다.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Dive
{
	static Dive::AppWindow* s_pAppWnd = nullptr;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (s_pAppWnd)
		{
			return s_pAppWnd->AppWndProc(hWnd, msg, wParam, lParam);
		}
		else
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
	}

	AppWindow::AppWindow(HINSTANCE hInstance, const WindowProps& props)
	{	
		DV_ASSERT(s_pAppWnd == nullptr);
		s_pAppWnd = this;

		if (hInstance == 0)
			hInstance = GetModuleHandle(NULL);
	
		std::wstring windowName;
		windowName.assign(props.Title.begin(), props.Title.end());

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

		int posX = (GetSystemMetrics(SM_CXSCREEN) - (int)props.Width) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - (int)props.Height) / 2;

		auto hWnd = CreateWindowW(windowName.c_str(), windowName.c_str(), WS_OVERLAPPEDWINDOW,
			posX, posY, props.Width, props.Height, nullptr, nullptr, hInstance, nullptr);

		DV_ASSERT(hWnd != NULL);

		m_Data.hInstance	= hInstance;
		m_Data.hWnd			= hWnd;
		m_Data.Title		= props.Title;
		m_Data.Width		= props.Width;
		m_Data.Height		= props.Height;
	}

	void AppWindow::Destroy()
	{
		DestroyWindow(m_Data.hWnd);
		m_Data.hWnd = nullptr;

		std::wstring windowName;
		windowName.assign(m_Data.Title.begin(), m_Data.Title.end());

		UnregisterClassW(windowName.c_str(), m_Data.hInstance);
		m_Data.hInstance = nullptr;
	}

	void AppWindow::Show()
	{
		DV_ASSERT(m_Data.hWnd);

		ShowWindow(m_Data.hWnd, SW_SHOW);
		SetForegroundWindow(m_Data.hWnd);
		SetFocus(m_Data.hWnd);
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

	LRESULT AppWindow::AppWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// 애매해...
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

		m_Data.Msg		= msg;
		m_Data.wParam	= wParam;
		m_Data.lParam	= lParam;

		switch (msg)
		{
		case WM_SIZE:
		{
			break;
		}
		case WM_CLOSE:
		{

			Event evnt;
			m_Data.EventCallback(evnt);

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
}