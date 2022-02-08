#pragma once

namespace Dive
{
	class Event;

	class Engine
	{
	public:
		Engine();
		virtual ~Engine();
		
		virtual void Initialize();
		virtual void Destroy();

		virtual void Tick();

		void OnEvent(Event& event);

	private:

	private:
		static Engine* s_pInstance;
	};
}