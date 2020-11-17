#pragma once
#include <Windows.h>
#include <string>
#include <functional>
#include "Core/Engine.h"
#include "ImGUI/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Window
{
	static HWND g_hWnd;
	static std::function<void(int width, int height)> g_OnResize;

	int GetWidth()
	{
		RECT rt;
		GetClientRect(g_hWnd, &rt);
		return static_cast<int>(rt.right - rt.left);
	}

	int GetHeight()
	{
		RECT rt;
		GetClientRect(g_hWnd, &rt);
		return static_cast<int>(rt.bottom - rt.top);
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		if (msg == WM_SIZE || msg == WM_DISPLAYCHANGE)
		{
				
			int width	= static_cast<int>(lParam & 0xffff);
			int height	= static_cast<int>((lParam >> 16) & 0xffff);

			if (g_OnResize)
				g_OnResize(width, height);
		}
		else if (msg == WM_CLOSE)
		{
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	bool Create(HINSTANCE hInstance, const std::wstring name)
	{
		std::wstring appName = std::move(name);
		std::wstring className{L"WndClass"};

		int windowWidth = GetSystemMetrics(SM_CXSCREEN);
		int windowHeight = GetSystemMetrics(SM_CYSCREEN);

		WNDCLASSEX wndClass;
		wndClass.cbSize			= sizeof(wndClass);
		wndClass.cbClsExtra		= 0;
		wndClass.cbWndExtra		= 0;
		wndClass.style			= 0;
		wndClass.hInstance		= hInstance;
		wndClass.lpfnWndProc	= WndProc;
		wndClass.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
		wndClass.hCursor		= LoadCursor(hInstance, IDC_ARROW);
		wndClass.hIcon			= LoadIcon(hInstance, IDI_APPLICATION);
		wndClass.hIconSm		= LoadIcon(hInstance, IDI_APPLICATION);
		wndClass.lpszClassName	= className.c_str();
		wndClass.lpszMenuName	= nullptr;

		if (!RegisterClassEx(&wndClass))
		{
			MessageBox(nullptr, L"Fail to register window class!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
			return false;
		}

		g_hWnd = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			className.c_str(),
			appName.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			windowWidth, windowHeight,
			nullptr, nullptr, hInstance, nullptr);

		if (!g_hWnd)
		{
			MessageBox(nullptr, L"Fail to create window!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
			return false;
		}

		return true;
	}

	void Destroy()
	{
		DestroyWindow(g_hWnd);
	}

	void Show()
	{
		ShowWindow(g_hWnd, SW_MAXIMIZE);
		UpdateWindow(g_hWnd);
		SetFocus(g_hWnd);
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
}