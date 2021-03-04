#include "Timer.h"
#include <chrono>

using namespace std;

chrono::high_resolution_clock::time_point startTime;

namespace Dive
{
	Timer::Timer()
	{
		Start();
		Record();
	}

	void Timer::Start()
	{
		startTime = chrono::high_resolution_clock::now();
	}

	double Timer::GetTotalTime()
	{
		auto now = chrono::high_resolution_clock::now();
		chrono::duration<double, std::milli> elapsed = now - startTime;

		return elapsed.count();
	}
	
	double Timer::GetElapsedTime()
	{
		return GetTotalTime() - m_lastTime;
	}

	void Timer::Record()
	{
		m_lastTime = GetTotalTime();
	}
}