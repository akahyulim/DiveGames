#pragma once

namespace Dive 
{
	class AppWindow
	{
	public:
		AppWindow(HINSTANCE hInstance, std::wstring name);
		~AppWindow();
			
		bool Initialize();
		void Destroy();
		bool Run();

		void Show();

		HWND GetHandle() const { return m_hWnd; }

	private:
		bool calcuSize();

	private:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		int m_width;
		int m_height;
		std::wstring m_name;

	};
}