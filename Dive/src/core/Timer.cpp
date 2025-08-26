#include "stdafx.h"
#include "Timer.h"
#include "EventDispatcher.h"

namespace Dive
{
	double Timer::s_elapsedTimeMS = 0;
	float Timer::s_deltaTimeMS = 0.0f;
	std::chrono::steady_clock::time_point Timer::s_lastTickTime;
	uint16_t Timer::s_fps = 0;

	void Timer::Initialize()
	{
		s_lastTickTime = std::chrono::steady_clock::now();
	}
	
	void Timer::Tick()
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
	}
}