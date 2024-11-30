#pragma once

namespace Dive
{
	class Window
	{
	public:
		Window();
		~Window() = default;

		bool Initialize(HINSTANCE hInstance, uint32_t width, uint32_t height, LPCWSTR pTitle);
		void Shutdown();

		bool Run();

		bool Resize(uint32_t width, uint32_t height, bool Windowed = true);

		DirectX::XMUINT2 GetSize();
		uint32_t GetWidth();
		uint32_t GetHeight();

		HWND GetHandle() { return m_hWnd; }

		LPCWSTR GetTitle() { return m_Title.c_str(); }
		void SetTitle(LPCWSTR title);

		bool IsWindowed() { return m_bWindowed; }

		void ShowWindow(int nCmdShow);

	private:

	private:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		std::wstring m_Title;
		bool m_bWindowed;
	};
}