#include "divepch.h"
#include "DvWindow.h"
#include "IO/DvLog.h"

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

namespace Dive 
{
	DvWindow::DvWindow()
		: m_hInstance(NULL),
		m_hWnd(NULL),
		m_Title(std::wstring()),
		m_Size(DirectX::XMINT2(0, 0)),
		m_Position(DirectX::XMINT2(0, 0)),
		m_Styles(0),
		m_Flags(0)
	{
	}

	DvWindow::~DvWindow()
	{
		Destroy();
	}

	bool DvWindow::Create(const std::wstring& title, int x, int y, int width, int height, unsigned int flags)
	{
		m_hInstance = GetModuleHandle(nullptr);

		WNDCLASSEX wc;
		wc.style = 0;
		wc.hInstance = m_hInstance;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = WND_CLASS_NAME;
		wc.cbSize = sizeof(WNDCLASSEX);
		
		RegisterClassEx(&wc);

		// 현재 해상도와 다른 값이 리턴된다.
		auto sizeX = GetSystemMetrics(SM_CXSCREEN);
		auto sizeY = GetSystemMetrics(SM_CYSCREEN);
		DV_LOG_ENGINE_DEBUG("Current Resolution: {0:d} x {1:d}", sizeX, sizeY);

		if ((width < 0 || width > sizeX) && (height < 0 || height > sizeY))
		{
			width = WND_WIDTH;
			height = WND_HEIGHT;
		}

		m_Styles = 0;
		if (flags & DV_WINDOW_BORDERLESS)
		{
			m_Styles = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		}
		else
		{
			m_Styles = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
			
			if (flags & DV_WINDOW_RESIZABLE)
			{
				m_Styles |= WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
			}
		}

		RECT rt = { 0, 0, width, height };
		::AdjustWindowRect(&rt, m_Styles, FALSE);

		m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, WND_CLASS_NAME, title.c_str(),
			m_Styles, x, y, rt.right - rt.left, rt.bottom - rt.top, NULL, NULL, m_hInstance, NULL);

		m_Title = title;
		m_Size = DirectX::XMINT2(width, height);
		m_Position = DirectX::XMINT2(x, y);
		m_Flags = flags;

		return true;
	}

	void DvWindow::Destroy()
	{
		if (!m_hWnd)
			return;

		if (IsFullScreen())
			::ChangeDisplaySettings(nullptr, 0);

		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;

		::UnregisterClassW(WND_CLASS_NAME, m_hInstance);
		m_hInstance = NULL;
	}

	bool DvWindow::Run()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		return msg.message != WM_QUIT ? true : false;
	}

	void DvWindow::ChangeTitle(const std::wstring& title)
	{
		if (!m_hWnd)
			return;

		::SetWindowText(m_hWnd, title.c_str());

		m_Title = title;
	}

	void DvWindow::Resize(int width, int height)
	{
		if (!m_hWnd)
			return;

		// 전체화면이라면 변경 불가.
		if (m_Flags & DV_WINDOW_FULLSCREEN)
			return;

		RECT rt = { 0, 0, width, height };
		::AdjustWindowRect(&rt, m_Styles, FALSE);

		::SetWindowPos(m_hWnd, NULL, m_Position.x, m_Position.y,
			rt.right - rt.left,
			rt.bottom - rt.top,
			SWP_SHOWWINDOW);

		m_Size.x = width;
		m_Size.y = height;

		::GetWindowRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Window Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		::GetClientRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Client Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);
	}

	void DvWindow::GetSize(int& outWidth, int& outHeight) const
	{
		outWidth = m_Flags & DV_WINDOW_FULLSCREEN ? ::GetSystemMetrics(SM_CXSCREEN) : m_Size.x;
		outHeight = m_Flags & DV_WINDOW_FULLSCREEN ? ::GetSystemMetrics(SM_CYSCREEN) : m_Size.y;
	}
	
	void DvWindow::SetPosition(int x, int y)
	{
		if (!m_hWnd)
			return;

		// 전체화면이라면 변경 불가.
		if (m_Flags & DV_WINDOW_FULLSCREEN)
			return;

		RECT rt{ 0, 0, m_Size.x, m_Size.y };
		::AdjustWindowRect(&rt, m_Styles, FALSE);

		::SetWindowPos(m_hWnd, NULL, x, y,
			rt.right - rt.left,
			rt.bottom - rt.top,
			SWP_SHOWWINDOW);
		
		m_Position.x = x;
		m_Position.y = y;

		::GetWindowRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Window Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		::GetClientRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Client Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);
	}

	void DvWindow::GetPosition(int& outX, int& outY) const
	{
		outX = m_Flags & DV_WINDOW_FULLSCREEN ? 0 : m_Position.x;
		outY = m_Flags & DV_WINDOW_FULLSCREEN ? 0 : m_Position.y;
	}

	void DvWindow::Show()
	{
		if (!m_hWnd)
			return;

		::ShowWindow(m_hWnd, SW_SHOW);
		::SetFocus(m_hWnd);
	}
	
	void DvWindow::Hide()
	{
		if (!m_hWnd)
			return;

		::ShowWindow(m_hWnd, SW_HIDE);
	}

	void DvWindow::Maximize()
	{
		if (!m_hWnd)
			return;

		::ShowWindow(m_hWnd, SW_MAXIMIZE);
		::SetFocus(m_hWnd);
	}
	
	void DvWindow::Minimize()
	{
		if (!m_hWnd)
			return;

		::ShowWindow(m_hWnd, SW_MINIMIZE);
	}
	
	void DvWindow::SetFullscreen(bool bFullscreen)
	{
		if (!m_hWnd)
			return;

		if (bFullscreen)
		{
			if (m_Flags & DV_WINDOW_FULLSCREEN)
				return;
			else
			{
				m_Flags |= DV_WINDOW_FULLSCREEN;

				// 기존 스타일로 돌아가기 위해 플래그는 변경하지 않는다.
				if (!(m_Flags & DV_WINDOW_BORDERLESS))
				{
					m_Styles = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
				}

				int currentResolutionX = ::GetSystemMetrics(SM_CXSCREEN);
				int currentResolutionY = ::GetSystemMetrics(SM_CYSCREEN);

				RECT rt{ 0, 0, currentResolutionX, currentResolutionY };
				::AdjustWindowRect(&rt, m_Styles, FALSE);
				::SetWindowLongPtr(m_hWnd, GWL_STYLE, m_Styles);

				// 기존 위치, 크기로 돌아가기 위해 해당 변수에 저장하지 않는다.
				::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, rt.right - rt.left, rt.bottom - rt.top, SWP_SHOWWINDOW);
			}
		}
		else
		{
			if (m_Flags & DV_WINDOW_FULLSCREEN)
			{
				m_Flags &= ~DV_WINDOW_FULLSCREEN;

				if (!(m_Flags & DV_WINDOW_BORDERLESS))
				{
					m_Styles = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
					if (m_Flags & DV_WINDOW_RESIZABLE)
					{
						m_Styles |= WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
					}
				}

				RECT rt{ 0, 0, m_Size.x, m_Size.y };
				::AdjustWindowRect(&rt, m_Styles, FALSE);
				::SetWindowLongPtr(m_hWnd, GWL_STYLE, m_Styles);
				::SetWindowPos(m_hWnd, NULL, m_Position.x, m_Position.y, rt.right - rt.left, rt.bottom - rt.top, SWP_SHOWWINDOW);
			}
			else
				return;
		}

		RECT rt;
		::GetWindowRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Window Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		::GetClientRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Client Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);
	}
	
	void DvWindow::SetBorderless(bool bBordered)
	{
		if (!m_hWnd)
			return;

		if (!(m_Flags & DV_WINDOW_FULLSCREEN))
		{
			if (bBordered)
			{
				if (m_Flags & DV_WINDOW_BORDERLESS)
					return;

				m_Flags |= DV_WINDOW_BORDERLESS;

				m_Styles = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			}
			else
			{
				if (m_Flags & DV_WINDOW_BORDERLESS)
					m_Flags &= ~DV_WINDOW_BORDERLESS;
				else
					return;

				m_Styles = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
				if (m_Flags & DV_WINDOW_RESIZABLE)
				{
					m_Styles |= WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
				}	
			}

			RECT rt{ 0, 0, m_Size.x, m_Size.y };
			::AdjustWindowRect(&rt, m_Styles, FALSE);
			::SetWindowLongPtr(m_hWnd, GWL_STYLE, m_Styles);
			::SetWindowPos(m_hWnd, NULL, m_Position.x, m_Position.y, rt.right - rt.left, rt.bottom - rt.top, SWP_SHOWWINDOW);
		}

		RECT rt;
		::GetWindowRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Window Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		::GetClientRect(m_hWnd, &rt);
		DV_LOG_ENGINE_DEBUG("Client Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);
	}
}