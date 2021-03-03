#include "Timer.h"

#include <chrono>

using namespace std;

chrono::high_resolution_clock::time_point startTime;

namespace Dive
{
	Dive_Timer::Dive_Timer()
	{
		Start();
		Record();
	}

	void Dive_Timer::Start()
	{
		startTime = chrono::high_resolution_clock::now();
	}

	double Dive_Timer::GetTotalTime()
	{
		auto now = chrono::high_resolution_clock::now();
		chrono::duration<double, std::milli> elapsed = now - startTime;

		return elapsed.count();
	}
	
	double Dive_Timer::GetElapsedTime()
	{
		return GetTotalTime() - m_lastTime;
	}

	void Dive_Timer::Record()
	{
		m_lastTime = GetTotalTime();
	}
}