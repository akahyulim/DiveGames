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
		* GetNumVideoDisplays(����� ����)
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
		// �ν��Ͻ�.
		HINSTANCE m_hInstance;
		// ������ �ڵ�.
		HWND m_hWnd;
		// Ÿ��Ʋ ����.
		std::wstring m_Title;
		// ������ ũ��.
		DirectX::XMINT2 m_Size;
		// ������ �»� ��ġ.
		DirectX::XMINT2 m_Position;
	};
}