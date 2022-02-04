#pragma once
#include "Renderer/Graphics/GraphicsDevice.h"
#include "Settings.h"

namespace Dive
{
	class AppWindow;

	class Runtime
	{
	public:
		Runtime(HINSTANCE hInstance, const std::string& title = "DiveGames");
		virtual ~Runtime();
		
		void Run();

		AppWindow& GetAppWindow() { return *m_pAppWindow; }

	protected:

	protected:
		static Runtime* s_pInstance;

		std::unique_ptr<AppWindow> m_pAppWindow;
	};
}