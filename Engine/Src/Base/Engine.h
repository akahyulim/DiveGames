#pragma once
#include "Time.h"

namespace Dive
{
	class Scene;
	class RenderPath;

	struct WindowData
	{
		HINSTANCE hInstance = 0;
		HWND hWnd			= 0;
		unsigned int msg	= 0;
		WPARAM wParam		= 0;
		LPARAM lParam		= 0;
		unsigned int Width	= 0;
		unsigned int Height = 0;
		bool bMinimize		= false;
		bool bMaximize		= false;

		bool bVSync			= false;
		bool bFullScreen	= false;
	};

	class Engine
	{
	public:
		Engine()	= default;
		~Engine()	= default;

		void Initialize(const WindowData* pData);
		void Shutdown();

		void Update();
		void Render();

		void SetWindowData(const WindowData& data);

		RenderPath* GetActiveRenderPath() { return m_pActiveRenderPath; }
		void SetActiveRenderPath(RenderPath* pRenderPath);

		Scene* CreateScene(const std::string& name = "");
		Scene* GetActiveScene() { return m_pActiveScene; }

	private:
	
	private:
		Time m_Time;
		WindowData m_WindowData;

		RenderPath* m_pActiveRenderPath = nullptr;

		Scene* m_pActiveScene = nullptr;
	};
}