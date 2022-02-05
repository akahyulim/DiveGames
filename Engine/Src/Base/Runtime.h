#pragma once
#include "Renderer/Graphics/GraphicsDevice.h"
#include "Settings.h"

namespace Dive
{
	class AppWindow;

	class Runtime
	{
	public:
		Runtime();
		virtual ~Runtime();

		virtual void Initialize();
		
		void Run();

		AppWindow* GetAppWindow() { return m_pAppWindow; }

	protected:

	protected:
		static Runtime* s_pInstance;

		AppWindow* m_pAppWindow = nullptr;
	};
}