#pragma once

namespace Dive
{
	class Engine
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void Tick();

		// time & fps
		static double GetElapsedTimeMS();
		static double GetElapsedTimeSec();
		static float GetDeltaTimeMS();
		static float GetDeltaTimeSec();
		static uint16_t GetFps();

	private:
		static double s_ElapsedTimeMS;
		static float s_DeltaTimeMS;
		static std::chrono::steady_clock::time_point s_LastTickTime;
		static uint16_t s_Fps;
	};
}
