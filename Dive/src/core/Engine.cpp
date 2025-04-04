#include "stdafx.h"
#include "Engine.h"
#include "CoreDefs.h"
#include "Window.h"
#include "graphics/Graphics.h"
#include "input/Input.h"
#include "rendering/Renderer.h"
#include "world/World.h"
#include "world/WorldManager.h"

namespace Dive
{
	double Engine::s_ElapsedTimeMS = 0;
	float Engine::s_DeltaTimeMS = 0.0f;
	std::chrono::steady_clock::time_point Engine::s_LastTickTime;
	uint16_t Engine::s_Fps = 0;

	// sprtan은 editor로부터 args를 받는다.
	void Engine::Initialize()
	{
		Window::Initialize();
		Graphics::Initialize();
		Renderer::Initialize();
		Input::Initialize();

		s_LastTickTime = std::chrono::steady_clock::now();

		DV_LOG(Engine, trace, "succed engine initialize");
	}

	void Engine::Shutdown()
	{
		Input::Shutdown();
		Renderer::Shutdown();
		Graphics::Shutdown();
		Window::Shutdown();

		DV_LOG(Engine, trace, "succed enigne shutdown");
		LogManager::Shutdown();
	}

	void Engine::Tick()
	{
		auto currentTickTime = std::chrono::steady_clock::now();
		s_DeltaTimeMS = std::chrono::duration<float, std::milli>(currentTickTime - s_LastTickTime).count();
		s_ElapsedTimeMS += s_DeltaTimeMS;
		s_LastTickTime = currentTickTime;

		static double lastTimeMS = 0;
		static uint16_t frameCount = 0;
		frameCount++;
		if (s_ElapsedTimeMS - lastTimeMS >= 1000.0)
		{
			s_Fps = frameCount;
			frameCount = 0;
			lastTimeMS = s_ElapsedTimeMS;
		}

		Input::Tick();
		auto activeWorld = WorldManager::GetActiveWorld();
		if(activeWorld)
			activeWorld->Update();
		Renderer::Tick();
	}

	double Engine::GetElapsedTimeMS()
	{
		return s_ElapsedTimeMS;
	}

	double Engine::GetElapsedTimeSec()
	{
		return s_ElapsedTimeMS / 1000.0;
	}

	float Engine::GetDeltaTimeMS()
	{
		return s_DeltaTimeMS;
	}

	float Engine::GetDeltaTimeSec()
	{
		return s_DeltaTimeMS / 1000.0f;
	}

	uint16_t Engine::GetFps()
	{
		return s_Fps;
	}
}
