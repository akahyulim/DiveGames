#pragma once

namespace Dive
{
	class AppWindow
	{
	public:
		AppWindow(HINSTANCE hInstance, const std::string& title = "DiveGames");
		~AppWindow();

		bool Run();

		void Destroy();

		HINSTANCE GetInstance() const { return m_hInstance; }
		HWND GetHandle() const { return m_hWnd; }

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

	private:
		HINSTANCE m_hInstance	= 0;
		HWND m_hWnd				= 0;
		
		std::string m_Title;
		
		unsigned int m_Width	= 800;
		unsigned int m_Height	= 600;
	};
}