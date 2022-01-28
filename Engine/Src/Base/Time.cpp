#include "divepch.h"
#include "Time.h"

namespace Dive
{
	std::chrono::high_resolution_clock::time_point Time::m_StartTime;

	double Time::m_RealTimeSinceStartup = 0;
	double Time::m_DeltaTime			= 0;
	float Time::m_FixedFrameRate		= 0.02f;
	float Time::m_TimeScale				= 1.0f;

	void Time::Initialize()
	{
		m_StartTime = std::chrono::high_resolution_clock::now();
	}

	void Time::Update()
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsedTime = currentTime - m_StartTime;

		m_RealTimeSinceStartup += elapsedTime.count();
		m_DeltaTime = elapsedTime.count() * m_TimeScale;

		m_StartTime = currentTime;
	}
}