#include "divepch.h"
#include "Window.h"
#include "Log.h"
#include "Engine.h"

namespace Dive
{
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

		return DvEngine::MessageHandler(hWnd, msg, wParam, lParam);
	}

	Window::Window()
		: m_hInstance(::GetModuleHandle(nullptr))
		, m_hWnd(nullptr)
		, m_ClassName(L"Dive Window")
		, m_Title(L"Dive Game")
	{
	}

	Window::~Window()
	{
		Release();
	}

	bool Window::Create(uint32_t width, uint32_t height, bool borderless)
	{
		WNDCLASSEX wc{};
		wc.style = 0;
		wc.hInstance = m_hInstance;
		wc.lpfnWndProc = DvWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = m_ClassName.c_str();
		wc.cbSize = sizeof(WNDCLASSEX);

		if (!::RegisterClassEx(&wc))
		{
			DV_ENGINE_ERROR("윈도우클래스 등록에 실패하였습니다.");
			return false;
		}

		DWORD style = borderless ? WS_POPUP : WS_OVERLAPPEDWINDOW;

		int posX = (GetSystemMetrics(SM_CXSCREEN) - static_cast<int>(width)) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - static_cast<int>(height)) / 2;

		m_hWnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			m_ClassName.c_str(),
			m_Title.c_str(),
			style,
			posX > 0 ? posX : 0,
			posY > 0 ? posY : 0,
			width, height,
			NULL, NULL,
			m_hInstance,
			NULL);

		if (!m_hWnd)
		{
			DV_ENGINE_ERROR("윈도우 생성에 실패하였습니다.");
			return false;
		}

		if (borderless)
			Resize(width, height, true);

		return true;
	}
	
	void Window::Release()
	{
		::ShowCursor(TRUE);

		::DestroyWindow(m_hWnd);
		m_hWnd = nullptr;

		::UnregisterClass(m_ClassName.c_str(), m_hInstance);
	}

	void Window::Resize(uint32_t width, uint32_t height, bool borderless)
	{
		if (!m_hWnd)
		{
			DV_ENGINE_WARN("제어할 윈도우가 존재하지 않습니다.");
			return;
		}

		DWORD style = borderless ? WS_POPUP : WS_OVERLAPPEDWINDOW;

		auto curStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
		if (curStyle != style)
			::SetWindowLongPtr(m_hWnd, GWL_STYLE, style);

		RECT rt = { 0, 0, (LONG)width, (LONG)height };
		::AdjustWindowRect(&rt, style, FALSE);

		int clientWidth = rt.right - rt.left;
		int clientHeight = rt.bottom - rt.top;

		int posX = (::GetSystemMetrics(SM_CXSCREEN) - clientWidth) / 2;
		int posY = (::GetSystemMetrics(SM_CYSCREEN) - clientHeight) / 2;

		::SetWindowPos(m_hWnd, NULL, posX, posY, clientWidth, clientHeight, SWP_DRAWFRAME);
	}
}