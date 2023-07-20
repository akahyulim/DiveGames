#include "DivePch.h"
#include "Timer.h"

namespace Dive
{
	static double s_TimeMS = 0.0;
	static double s_DeltaTimeMS = 0.0;

	static std::chrono::steady_clock::time_point s_LastTickTime;

	void Timer::Update()
	{
		if (s_LastTickTime.time_since_epoch() != std::chrono::steady_clock::duration::zero())
			s_DeltaTimeMS = static_cast<double>(std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - s_LastTickTime).count());

		s_TimeMS += s_DeltaTimeMS;
		s_LastTickTime = std::chrono::steady_clock::now();
	}

	double Timer::GetTimeMS()
	{
		return s_TimeMS;
	}

	double Timer::GetTimeSec()
	{
		return s_TimeMS / 1000.0;
	}
	
	double Timer::GetDeltaTimeMS()
	{
		return s_DeltaTimeMS;
	}
	
	double Timer::GetDeltaTimeSec()
	{
		return s_DeltaTimeMS / 1000.0;
	}
}