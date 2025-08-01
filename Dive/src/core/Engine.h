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

		static double GetElapsedTimeMS() { return s_elapsedTimeMS; }
		static double GetElapsedTimeSec() { return s_elapsedTimeMS / 1000.0; }
		
		static float GetDeltaTimeMS() { return s_deltaTimeMS; }
		static float GetDeltaTimeSec() { return s_deltaTimeMS / 1000.0f; }
		
		static uint16_t GetFps() { return s_fps; }

		static uint64_t GetFrameCount() { return s_frameCount; }

	private:
		static double s_elapsedTimeMS;
		static float s_deltaTimeMS;
		static std::chrono::steady_clock::time_point s_lastTickTime;
		static uint16_t s_fps;
		static uint64_t s_frameCount;
	};
}
