#include "stdafx.h"
#include "Engine.h"
#include "CoreDefs.h"
#include "Window.h"
#include "Input.h"
#include "graphics/Graphics.h"
#include "graphics/ShaderManager.h"
#include "rendering/Renderer.h"
#include "world/World.h"
#include "resource/ResourceManager.h"

namespace Dive
{
	double Engine::s_ElapsedTimeMS = 0;
	float Engine::s_DeltaTimeMS = 0.0f;
	std::chrono::steady_clock::time_point Engine::s_LastTickTime;
	uint16_t Engine::s_Fps = 0;
	uint64_t Engine::s_FrameCount = 0;

	void Engine::Initialize()
	{
		DV_LOG(Engine, info, "초기화 시작 ---------------------------------------");

		Window::Initialize();
		Graphics::Initialize();
		ShaderManager::Initialize();
		Renderer::Initialize();
		Input::Initialize();

		s_LastTickTime = std::chrono::steady_clock::now();

		DV_LOG(Engine, info, "초기화 완료 ---------------------------------------");
	}

	void Engine::Shutdown()
	{
		DV_LOG(Engine, info, "셧다운 시작 ---------------------------------------");

		WorldManager::Clear();
		ResourceManager::Clear();
		Input::Shutdown();
		Renderer::Shutdown();
		Graphics::Shutdown();
		Window::Shutdown();

		DV_LOG(Engine, info, "셧다운 완료 ---------------------------------------");

		// 현재 문제없이 종료된다.
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

		if (WorldManager::GetActiveWorld())
			WorldManager::GetActiveWorld()->Update();

		Renderer::OnUpdate();

		s_FrameCount++;
	}
}
