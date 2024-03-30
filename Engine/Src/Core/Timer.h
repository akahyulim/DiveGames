#pragma once

namespace Dive
{
	class Timer
	{
	public:
		static void Update();

		static double GetFps();

		static double GetTimeMS();
		static double GetTimeSec();

		static double GetDeltaTimeMS();
		static double GetDeltaTimeSec();
	};
}