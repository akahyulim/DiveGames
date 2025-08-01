#include "stdafx.h"
#include "Engine.h"
#include "Window.h"
#include "Input.h"
#include "graphics/Graphics.h"
#include "graphics/ShaderManager.h"
#include "rendering/Renderer.h"
#include "world/World.h"
#include "resource/ResourceManager.h"

namespace Dive
{
	double Engine::s_elapsedTimeMS = 0;
	float Engine::s_deltaTimeMS = 0.0f;
	std::chrono::steady_clock::time_point Engine::s_lastTickTime;
	uint16_t Engine::s_fps = 0;
	uint64_t Engine::s_frameCount = 0;

	bool Engine::Initialize()
	{
		DV_LOG(Engine, info, "초기화 시작");

		if (!Window::Initialize())
		{
			DV_LOG(Engine, err, "[::Initialize] Window 초기화 실패");
			return false;
		}

		if (!Graphics::Initialize())
		{
			DV_LOG(Engine, err, "[::Initialize] Graphics 초기화 실패");
			return false;
		}

		if (!ShaderManager::Initialize(Graphics::GetDevice()))
		{
			DV_LOG(Engine, err, "[::Initialize] ShaderManager 초기화 실패");
			return false;
		}

		Renderer::Initialize();

		if (!Input::Initialize())
		{
			DV_LOG(Engine, err, "[::Initialize] Input 초기화 실패");
			return false;
		}

		s_lastTickTime = std::chrono::steady_clock::now();
		DV_LOG(Engine, info, "초기화 완료");

		return true;
	}

	void Engine::Shutdown()
	{
		DV_LOG(Engine, info, "셧다운 시작");

		WorldManager::Clear();
		ResourceManager::Clear();
		ShaderManager::Clear();
		Input::Shutdown();
		Renderer::Shutdown();
		Graphics::Shutdown();
		Window::Shutdown();

		DV_LOG(Engine, info, "셧다운 완료");

		LogManager::Shutdown();
	}

	void Engine::Tick()
	{
		auto currentTickTime = std::chrono::steady_clock::now();
		s_deltaTimeMS = std::chrono::duration<float, std::milli>(currentTickTime - s_lastTickTime).count();
		s_elapsedTimeMS += s_deltaTimeMS;
		s_lastTickTime = currentTickTime;

		static double lastTimeMS = 0;
		static uint16_t frameCount = 0;
		frameCount++;
		if (s_elapsedTimeMS - lastTimeMS >= 1000.0)
		{
			s_fps = frameCount;
			frameCount = 0;
			lastTimeMS = s_elapsedTimeMS;
		}

		if (WorldManager::GetActiveWorld())
			WorldManager::GetActiveWorld()->Update();

		Renderer::OnUpdate();

		s_frameCount++;
	}
}
