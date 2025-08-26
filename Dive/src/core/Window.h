#pragma once

namespace Dive
{
	class Window
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static bool Run();

		static void SetMessageCallback(LONG_PTR callBack);

		static bool Resize(const uint32_t width, const uint32_t height);
		static uint32_t GetWidth();
		static uint32_t GetHeight();

		static HINSTANCE GetInstanceHandle() { return s_hInstance; }
		static HWND GetWindowHandle() { return s_hWnd; }
		
		static void Show();
		static void Hide();
		static void Close();
		
		static bool IsFullScreen();
		static void FullScreen();
		
		static void SetWindowed(bool windowed = true);

		static void Minimize();
		static void Maximize();
		static void Restore();
		static bool IsMaximize();

		static bool SetTitle(const std::wstring& title);

	private:
		static HINSTANCE s_hInstance;
		static HWND s_hWnd;
		static std::wstring s_title;
	};
}
