#include "stdafx.h"
#include "Engine.h"
#include "CoreDefs.h"
#include "Window.h"
#include "Input.h"
#include "transforms/Transforms.h"
#include "graphics/Graphics.h"
#include "rendering/Renderer.h"
#include "world/World.h"

namespace Dive
{
	double Engine::s_ElapsedTimeMS = 0;
	float Engine::s_DeltaTimeMS = 0.0f;
	std::chrono::steady_clock::time_point Engine::s_LastTickTime;
	UINT16 Engine::s_Fps = 0;
	UINT64 Engine::s_FrameCount = 0;
	std::shared_ptr<World> Engine::s_ActiveWorld;

	// sprtan은 editor로부터 args를 받는다.
	void Engine::Initialize()
	{
		Window::Initialize();
		Transforms::Initialize();
		Graphics::Initialize();
		Renderer::Initialize();
		Input::Initialize();

		s_LastTickTime = std::chrono::steady_clock::now();
	}

	void Engine::Shutdown()
	{
		Input::Shutdown();
		Renderer::Shutdown();
		Graphics::Shutdown();
		Transforms::Shutdown();
		Window::Shutdown();
		LogManager::Shutdown();
	}

	void Engine::Tick()
	{
		auto currentTickTime = std::chrono::steady_clock::now();
		s_DeltaTimeMS = std::chrono::duration<float, std::milli>(currentTickTime - s_LastTickTime).count();
		s_ElapsedTimeMS += s_DeltaTimeMS;
		s_LastTickTime = currentTickTime;

		static double lastTimeMS = 0;
		static UINT16 frameCount = 0;
		frameCount++;
		if (s_ElapsedTimeMS - lastTimeMS >= 1000.0)
		{
			s_Fps = frameCount;
			frameCount = 0;
			lastTimeMS = s_ElapsedTimeMS;
		}

		if (s_ActiveWorld)
		{
			// 결국 엔진에서 Editor와 Runtime을 구분해야 한다.
			s_ActiveWorld->EditorTick(s_DeltaTimeMS);
		}

		s_FrameCount++;
	}

	std::shared_ptr<World> Engine::GenerateWorld(const std::string& name)
	{
		s_ActiveWorld = std::make_shared<World>(name);
		return s_ActiveWorld;
	}
}
