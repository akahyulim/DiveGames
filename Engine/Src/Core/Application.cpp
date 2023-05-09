#include "DivePch.h"
#include "Application.h"
#include "Engine.h"

namespace Dive
{
	Application::Application()
	{
	}

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
				Engine::RunFrame();

			Stop();
		}

		return 0;
	}
}