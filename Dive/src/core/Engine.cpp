#include "stdafx.h"
#include "Engine.h"
#include "CoreDefs.h"
#include "Window.h"
#include "transforms/Transforms.h"
#include "graphics/Graphics.h"
#include "input/Input.h"
#include "rendering/Renderer.h"

// hazel은 Engine이 없다.
// 대신 App에서 기본 시스템들을 생성 및 셧다운을 수행하고
// 런타임은 Layer에서 App에 맞춰 구현한다.
// Layer는 World을 생성 및 실행하고
// World은 Tick에서 Script Component 업데이트, Physics, Renderer 시스템을 수행한다.
// 여기에서 이해되지 않는 부분은 Layer를 Stack으로 관리한다는 것이다.
// 덧붙여 World에서 system들을 사용하는 것도 ecs 관점에서 적절하지 않은 듯 하다.
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
		Transforms::Initialize();
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
		Transforms::Shutdown();
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

		// hazel의 경우 World에서 이들을 처리하는 듯 하다.
		// 허나 이를 따를 필요는 없어보인다.
		Transforms::Tick();
		Input::Tick();
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
