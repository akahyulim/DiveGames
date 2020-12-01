#pragma once

namespace Dive
{
	class Window
	{
	public:
		Window();
		~Window() = default;

		bool Create(int width, int height);
		void Destroy();

		bool Run();

		void Show();


		// option
		// set icon
		// set title

	private:
	private:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		std::wstring m_title;
		int m_width;
		int m_height;
		bool m_bFullScreen;
	};
}