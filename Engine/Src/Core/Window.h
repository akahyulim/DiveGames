#pragma once

namespace Dive
{
	class Window
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static bool Run();

		static bool Resize(uint32_t width, uint32_t height, bool Windowed = true);

		static void SetWndProc(WNDPROC wndProc);

		static DirectX::XMUINT2 GetSize();
		static uint32_t GetWidth();
		static uint32_t GetHeight();

		static HWND GetHandle() { return s_hWnd; }

		static LPCWSTR GetTitle() { return s_Title.c_str(); }
		static void SetTitle(LPCWSTR title);

		static bool IsWindowed() { return s_bWindowed; }

		static void ShowWindow(int nCmdShow);

	private:

	private:
		static HINSTANCE s_hInstance;
		static HWND s_hWnd;
		static WNDPROC s_WndProc;
		static std::wstring s_Title;
		static bool s_bWindowed;
	};
}