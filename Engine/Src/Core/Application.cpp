#include "DivePch.h"
#include "Application.h"
#include "Engine.h"
#include "Log.h"


namespace Dive
{
	Application::Application()
	{
		m_engine = std::make_shared<Engine>();
	}

	int Application::Run()
	{
		Setup();
		// engine intialize
		APP_TRACE("Setup Application & Initialize Engine!");

		Start();
		// while engine run
		APP_TRACE("Start Application & Run Engine");

		Stop();
		APP_TRACE("Stop Application & Destroy Engine");

		return 1;
	}
}