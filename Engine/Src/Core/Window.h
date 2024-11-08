#pragma once

namespace Dive
{
	class Window
	{
	public:
		Window();
		~Window();

		bool Create(uint32_t width, uint32_t height, bool borderless = false);
		void Release();

		void Resize(uint32_t width, uint32_t height, bool borderless = false);

		HWND GetHandle() const { return m_hWnd; }

		LPCWSTR GetClassName() const { return m_ClassName.c_str(); }
		void SetClassName(LPCWSTR name) { m_ClassName = name; }

		LPCWSTR GetTitle() const { return m_Title.c_str(); }
		void SetTitle(LPCWSTR title) { m_Title = title; }

	private:
	private:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		std::wstring m_ClassName;
		std::wstring m_Title;
	};
}