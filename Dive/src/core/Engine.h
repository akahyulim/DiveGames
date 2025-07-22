#pragma once

namespace Dive
{
	class World;

	class Engine
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static void Tick();

		static double GetElapsedTimeMS() { return s_ElapsedTimeMS; }
		static double GetElapsedTimeSec() { return s_ElapsedTimeMS / 1000.0; }
		
		static float GetDeltaTimeMS() { return s_DeltaTimeMS; }
		static float GetDeltaTimeSec() { return s_DeltaTimeMS / 1000.0f; }
		
		static uint16_t GetFps() { return s_Fps; }

		static uint64_t GetFrameCount() { return s_FrameCount; }

	private:
		static double s_ElapsedTimeMS;
		static float s_DeltaTimeMS;
		static std::chrono::steady_clock::time_point s_LastTickTime;
		static uint16_t s_Fps;
		static uint64_t s_FrameCount;
	};
}
