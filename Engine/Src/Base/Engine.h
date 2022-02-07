#pragma once

namespace Dive
{
	struct WindowData
	{
		HWND hWnd = nullptr;
		UINT msg = 0;
		WPARAM wParam = 0;
		LPARAM lParam = 0;
		unsigned int Width = 0;
		unsigned int Height = 0;
		bool Minimize = false;
		bool Maximize = false;
	};

	class Engine
	{
	public:
		Engine();
		~Engine();
		
		void Tick();

		const WindowData& GetWindowData() const { return m_WindowData; }
		void SetWindowData(WindowData& data) { m_WindowData = data; }

	private:

	private:
		static Engine* s_pInstance;

		WindowData m_WindowData;
	};
}