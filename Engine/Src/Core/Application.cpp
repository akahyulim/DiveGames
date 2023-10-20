#include "DivePch.h"
#include "Application.h"
#include "Engine.h"

namespace Dive
{
	Application::~Application()
	{
		Engine::Shutdown();
	}

	int Application::Run()
	{
		Setup();

		if (Engine::Initialize())
		{
			Start();

			while (!Engine::IsExiting())
				Engine::Tick();

			Stop();
		}

		return 0;
	}
}