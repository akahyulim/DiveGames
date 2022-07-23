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
		: m_hInstance(nullptr),
		m_hWnd(nullptr),
		m_Title(std::wstring()),
		m_Size(DirectX::XMINT2(0, 0)),
		m_Position(DirectX::XMINT2(0, 0))
	{
	}

	DvWindow::~DvWindow()
	{
	}

	bool DvWindow::DvCreateWindow(const std::wstring& title, int x, int y, int width, int height, unsigned int flags)
	{
		m_hInstance = GetModuleHandle(nullptr);

		WNDCLASSEX wc;
		wc.style = 0;
		wc.hInstance = m_hInstance;
		wc.lpfnWndProc = WndProc;		// 프로시저 함수 역시 멤버 변수로 두고 생성전에 설정하는 건 어떨까?
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = L"AppWnd";
		wc.cbSize = sizeof(WNDCLASSEX);
		
		RegisterClassEx(&wc);

		// 현재 해상도와 다른 값이 리턴된다.
		auto sizeX = GetSystemMetrics(SM_CXSCREEN);
		auto sizeY = GetSystemMetrics(SM_CYSCREEN);
		DV_LOG_ENGINE_DEBUG("Current Resolution: {0:d} x {1:d}", sizeX, sizeY);

		if ((width < 0 || width > sizeX) && (height < 0 || height > sizeY))
		{
			width = 800;
			height = 600;
		}

		if (x < 0 || x > sizeX)
			x = (sizeX - width) / 2;
		if (y < 0 || y > sizeY)
			y = (sizeY - height) / 2;

		m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, L"AppWnd", title.c_str(),
			WS_OVERLAPPEDWINDOW,
			x, y, width, height, NULL, NULL, m_hInstance, NULL);

		ShowWindow(m_hWnd, SW_SHOW);
		SetFocus(m_hWnd);

		m_Title = title;
		m_Size = DirectX::XMINT2(width, height);	// Client Rect 크기가 아니다.
		m_Position = DirectX::XMINT2(x, y);

		return true;
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

	void DvWindow::SetWindowTitle(const std::wstring& title)
	{
		// 애매하다. 윈도우를 생성하기 전에 설정하면 의미가 없다.
		// 반면 윈도우를 생성한 후라면 변경할 수 있어야 한다.
		// => 윈도우를 생성한 후 호출하는 것이라 보아야 한다.
	}
	
	void DvWindow::SetWindowPosition(int x, int y)
	{
		// 위와 마찬가지다. 생성한 후 변경의 목적으로 만들어야 한다.
	}
}