/*
	Window

	제작: 서보윤( akahyulim@gmail.com )
*/
#pragma once

//= INCLUDES ==================
#include <Windows.h>
#include <functional>
#include "core/DiveEngine.h"
//=============================

namespace Window
{
	static HWND g_HandleWindow			= nullptr;
	static HINSTANCE g_HandleInstance	= nullptr;

	std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> g_OnMessage;
	std::function<void(int, int)> g_OnResize;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//g_OnMessage(hWnd, msg, wParam, lParam);

		LRESULT result = 0;

		if ((msg == WM_SIZE) || (msg == WM_DISPLAYCHANGE))
		{
			//g_OnResize(static_cast<int>(lParam & 0xffff), static_cast<int>((lParam >> 16) & 0xffff));
		}
		else if (msg == WM_CLOSE)
		{
			PostQuitMessage(0);
		}
		else
		{
			result = DefWindowProc(hWnd, msg, wParam, lParam);
		}

		return result;
	}

	bool Create(HINSTANCE handleInstance, const std::wstring& title, int width = 0, int height = 0)
	{
		g_HandleInstance = handleInstance ? handleInstance : (HINSTANCE)::GetModuleHandle(nullptr);

		LPCWSTR pClassName = L"WndClass";

		if (width == 0 || height == 0)
		{
			width = GetSystemMetrics(SM_CXSCREEN);
			height = GetSystemMetrics(SM_CYSCREEN);
		}

		int pos_x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		int pos_y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		WNDCLASSEX wc;
		wc.style = 0;
		wc.lpfnWndProc		= WndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= g_HandleInstance;
		wc.hIcon			= LoadIcon(nullptr, IDI_APPLICATION);
		wc.hIconSm			= wc.hIcon;
		wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground	= (HBRUSH)COLOR_WINDOW;
		wc.lpszMenuName		= nullptr;
		wc.lpszClassName	= pClassName;
		wc.cbSize			= sizeof(WNDCLASSEX);

		if (!RegisterClassEx(&wc))
		{
			MessageBox(nullptr, L"윈도우 클래스 등록에 실패하였습니다.", L"Error", MB_OK);
			return false;
		}

		g_HandleWindow = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			pClassName,
			title.c_str(),
			WS_OVERLAPPEDWINDOW,
			pos_x, pos_y,
			width, height,
			nullptr, nullptr, g_HandleInstance, nullptr);

		if (!g_HandleWindow)
		{
			MessageBox(nullptr, L"윈도우 생성에 실패하였습니다.", L"Error", MB_OK);
			return false;
		}

		return true;
	}

	void Destroy()
	{
		DestroyWindow(g_HandleWindow);
		g_HandleWindow = NULL;

		UnregisterClass(L"WndClass", g_HandleInstance);
		g_HandleInstance = NULL;
	}

	void Show()
	{
		ShowWindow(g_HandleWindow, SW_MAXIMIZE);
		UpdateWindow(g_HandleWindow);
		SetFocus(g_HandleWindow);
	}

	bool Run()
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

	int GetWidth()
	{
		RECT rt;
		GetClientRect(g_HandleWindow, &rt);

		return static_cast<int>(rt.right - rt.left);
	}

	int GetHeight()
	{
		RECT rt;
		GetClientRect(g_HandleWindow, &rt);

		return static_cast<int>(rt.bottom - rt.top);
	}

	void GetWindowSize(float* width, float* height)
	{
		RECT rt;
		GetClientRect(g_HandleWindow, &rt);

		*width = static_cast<float>(rt.right - rt.left);
		*height = static_cast<float>(rt.bottom - rt.top);
	}
}