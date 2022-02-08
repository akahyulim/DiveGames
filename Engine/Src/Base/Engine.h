#pragma once
#include "Events/Event.h"
#include "Events/AppEvent.h"

namespace Dive
{

	class Engine
	{
	public:
		Engine();
		virtual ~Engine();
		
		virtual void Initialize();
		virtual void Destroy();

		virtual void Tick();

		void OnEvent(Event& event);
		bool OnWinodwResize(WindowResizeEvent& event);

	private:

	private:
		static Engine* s_pInstance;
	};
}