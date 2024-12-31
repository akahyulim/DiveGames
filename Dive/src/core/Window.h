#pragma once

namespace Dive
{
	struct WindowData
	{
		WindowData(const std::wstring& title = L"dive", uint32_t width = 1600, uint32_t height = 900)
			: Title(title), Width(width), Height(height)
		{}

		std::wstring Title;
		uint32_t Width;
		uint32_t Height;
	};

	class Window
	{
	public:
		Window();
		Window(const WindowData& data);
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

		bool ChangeProc(LONG_PTR pWndProc);

		static std::unique_ptr<Window> Create(const WindowData& data = WindowData());

	private:
		bool initialize();
		void shutdown();

	private:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		std::wstring m_Title;
		bool m_bWindowed;
		uint32_t m_Width;
		uint32_t m_Height;
	};
}