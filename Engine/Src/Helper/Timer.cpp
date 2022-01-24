#include "Timer.h"

using namespace std;


namespace DiveEngine
{
	Timer::Timer()
	{
		Reset();
	}

	void Timer::Reset()
	{
		m_StartTime = chrono::high_resolution_clock::now();
	}

	double Timer::GetElapsedTime(bool reset)
	{
		auto currentTime = chrono::high_resolution_clock::now();
		chrono::duration<double, std::milli> elapsedTime = currentTime - m_StartTime;

		if (reset)
		{
			m_StartTime = currentTime;
		}

		return elapsedTime.count();
	}

	void TimeManager::Initialize()
	{
		m_StartTime = chrono::high_resolution_clock::now();
	}

	void TimeManager::Update()
	{
		auto currentTime = chrono::high_resolution_clock::now();
		chrono::duration<double, std::milli> elapsedTime = currentTime - m_StartTime;

		m_RealTimeSinceStartup += elapsedTime.count();
		m_DeltaTime = elapsedTime.count() * m_TimeScale;

		m_StartTime = currentTime;

		++m_FrameCount;
	}
}