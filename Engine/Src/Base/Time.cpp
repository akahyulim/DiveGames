#include "divepch.h"
#include "Time.h"

namespace Dive
{
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