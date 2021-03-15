#include "Timer.h"

using namespace std;


namespace Dive
{
	Timer::Timer()
	{
		Reset();
	}

	void Timer::Reset()
	{
		m_startTime = chrono::high_resolution_clock::now();
	}

	double Timer::GetElapsedTime(bool reset)
	{
		auto currentTime = chrono::high_resolution_clock::now();
		chrono::duration<double, std::milli> elapsedTime = currentTime - m_startTime;

		if (reset)
		{
			m_startTime = currentTime;
		}

		return elapsedTime.count();
	}

	void TimeManager::Initialize()
	{
		m_startTime = chrono::high_resolution_clock::now();
	}

	void TimeManager::Update()
	{
		auto currentTime = chrono::high_resolution_clock::now();
		chrono::duration<double, std::milli> elapsedTime = currentTime - m_startTime;

		m_realTimeSinceStartup += elapsedTime.count();
		m_deltaTime = elapsedTime.count();

		m_startTime = currentTime;

		++m_frameCount;
	}
}