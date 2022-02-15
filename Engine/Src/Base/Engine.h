#pragma once
#include "Time.h"
#include "Renderer/Renderer.h"

namespace Dive
{
	struct WindowData
	{
		HWND hWnd = 0;
		unsigned int Width = 0;
		unsigned int Height = 0;
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

		Renderer& GetRenderer() { return m_Renderer; }

	private:
	
	private:
		Time m_Time;

		Renderer m_Renderer;
	};
}