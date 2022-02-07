#pragma once
#include <Windows.h>
#include <string>
#include <functional>

namespace Dive
{
	struct EditorData;
	struct WindowData;

	static std::function<void(WindowData& data)> g_pOnMessage;

	class AppWindow
	{
	public:
		AppWindow(HINSTANCE hInstance, const EditorData& data);
		~AppWindow() = default;

		void Create();
		void Destroy();

		void Show();

		bool Run();

		HINSTANCE GetInstance() const { return m_hInstance; }
		void SetInstance(HINSTANCE hInstance) { m_hInstance = hInstance; }

		HWND GetHandle() const { return m_hWnd; }

		std::string GetTitle() const { return m_Title; }
		void SetTitle(const std::string& title) { m_Title = title; }

		unsigned int GetWidth() const { return m_Width; }
		void SetWidth(unsigned int width) { m_Width = width; }

		unsigned int GetHeight() const { return m_Height; }
		void SetHeight(unsigned int height) { m_Height = height; }


	private:
		HINSTANCE m_hInstance	= 0;
		HWND m_hWnd				= 0;
		
		std::string m_Title		= "Dive";
		
		unsigned int m_Width	= 800;
		unsigned int m_Height	= 600;

		bool m_bMaximize		= false;
	};
}