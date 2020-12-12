#pragma once

namespace Dive
{
	class Window
	{
	public:
		Window();
		~Window() = default;

		bool Create(int width, int height, bool fullScreen, bool windowed);
		void Destroy();

		bool Run();

		void Show(bool show);

		HWND GetHandle() const { return m_hWnd; }

		int GetWidth() const { return m_width; }
		int GetHeight() const { return m_height; }
		int GetClientRectWidth();
		int GetClientRectHeight();

		// set icon
		// set title
		bool SetTitle(std::wstring title);

	private:
	private:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		std::wstring m_title;
		int m_width;
		int m_height;
		DWORD m_style;
		int m_positionX;
		int m_positionY;
	};
}