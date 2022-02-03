#pragma once
#include <Windows.h>
#include <iostream>

namespace Dive
{
	struct WindowData
	{
		WindowData(const std::wstring& title = L"DiveGames",
			unsigned int width = 800,
			unsigned int height = 600)
			: Title(title), Width(width), Height(height)
		{
		}

		std::wstring Title;
		unsigned int Width;
		unsigned int Height;
	};

	class Window
	{
	public:
		Window(HINSTANCE hInstance, const WindowData& data);
		~Window();

		bool Run();

		void Destroy();

		unsigned int GetWidth() const { return m_Data.Width; }
		unsigned int GetHeight() const { return m_Data.Height; }

		HWND GetHandle() { return m_hWnd; }

	private:
		WindowData m_Data;

		HINSTANCE m_hInstance;
		HWND m_hWnd;
	};
}