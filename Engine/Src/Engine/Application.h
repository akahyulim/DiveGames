#pragma once
#include "Core/Main.h"

namespace Dive
{
	class Engine;

	class Application
	{
	public:
		Application();
		virtual ~Application() = default;

		virtual void Setup() {}
		virtual void Start() {}
		virtual void Stop() {}

		virtual int Run();

	protected:
		std::shared_ptr<Engine> m_engine;
	};
}


// main function macro
#define DIVE_APPLICATION_MAIN(className)		\
int RunApplication()							\
{												\
	className app;								\
	return app.Run();							\
}												\
DIVE_DEFINE_MAIN(RunApplication())