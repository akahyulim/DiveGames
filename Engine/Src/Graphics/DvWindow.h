#pragma once
#include <Windows.h>

namespace Dive
{
	class DvWindow
	{
	public:
		DvWindow();
		~DvWindow();

		bool DvCreateWindow(const std::wstring& title, int x, int y, int width, int height, unsigned int flags);

		bool Run();

		void SetWindowTitle(const std::wstring& title);
		void SetWindowPosition(int x, int y);

		HINSTANCE GetInstance() const { return m_hInstance; }
		HWND GetWindowHandle() const { return m_hWnd; }

		/*
		* GetNumDisplayMode
		* GetDesktopDisplayMode
		* GetNumVideoDisplays(모니터 개수)
		* GetWindowDisplayIndex
		* GetWindowFlags
		* GetDisplayDPI
		* MaximizeWindow / MinimizeWindow
		* ResizeWindow / GetWindowSize
		* SetWindowIcon
		* FreeSurface
		* SetWindowFullScreen
		* SetWindowBordered
		* ShowWindow
		* DestroyWindow
		*/

	private:
		// 인스턴스.
		HINSTANCE m_hInstance;
		// 윈도우 핸들.
		HWND m_hWnd;
		// 타이틀 문구.
		std::wstring m_Title;
		// 윈도우 크기.
		DirectX::XMINT2 m_Size;
		// 윈도우 좌상 위치.
		DirectX::XMINT2 m_Position;
	};
}