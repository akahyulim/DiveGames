#include "stdafx.h"
#include "Engine.h"
#include "CoreDefs.h"
#include "Window.h"
#include "Input.h"
#include "graphics/Graphics.h"
#include "rendering/Renderer.h"
#include "world/World.h"
#include "resource/ResourceManager.h"

namespace Dive
{
	double Engine::s_ElapsedTimeMS = 0;
	float Engine::s_DeltaTimeMS = 0.0f;
	std::chrono::steady_clock::time_point Engine::s_LastTickTime;
	UINT16 Engine::s_Fps = 0;
	UINT64 Engine::s_FrameCount = 0;

	// sprtan은 editor로부터 args를 받는다.
	void Engine::Initialize()
	{
		Window::Initialize();
		Graphics::Initialize();
		Renderer::Initialize();
		Input::Initialize();

		s_LastTickTime = std::chrono::steady_clock::now();
	}

	void Engine::Shutdown()
	{
		// 임시: 윈도우 X 버튼을 눌렀을 때 대비
		// =>  위치는 이 곳이 맞는 것 같다.
		// 기존 MenuBar Exit에서 World를 Clear하는 게 이상했다.
		if (WorldManager::GetActiveWorld())
			WorldManager::GetActiveWorld()->Clear();

		ResourceManager::Clear();
		Input::Shutdown();
		Renderer::Shutdown();
		Graphics::Shutdown();
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

		if (WorldManager::GetActiveWorld())
			WorldManager::GetActiveWorld()->Update();
	
		s_FrameCount++;
	}
}
