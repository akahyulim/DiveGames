#pragma once
#include "Time.h"
#include "Renderer/Renderer.h"

namespace Dive
{
	struct WindowData
	{
		HINSTANCE hInstance = 0;
		HWND hWnd = 0;
		unsigned int msg = 0;
		WPARAM wParam = 0;
		LPARAM lParam = 0;
		unsigned int Width = 0;
		unsigned int Height = 0;
		bool bMinimize = false;
		bool bMaximize = false;

		bool bVSync = false;
		bool bFullScreen = false;
	};

	class Engine
	{
	public:
		Engine()	= default;
		~Engine()	= default;

		void Initialize(const WindowData* pData);
		void Shutdown();

		void Run();

		void SetWindowData(const WindowData& data);

	private:
	
	private:
		Time m_Time;
		WindowData m_WindowData;
	};
}