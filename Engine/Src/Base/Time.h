#pragma once

namespace Dive
{
	class Time
	{
	public:
		static void Initialize();

		static void Update();

		static double GetDeltaTimeMS() { return m_DeltaTime; }
		static float GetDeltaTimeSec() { return static_cast<float>(m_DeltaTime / 1000.0f); }
 
		static double GetRealTimeSinceStartUpMS() { return m_RealTimeSinceStartup; }
		static unsigned int GetRealTimeSinceStartUpSec() { return static_cast<unsigned int>(m_RealTimeSinceStartup / 1000); }

		static float GetFixedFrameRate() { return m_FixedFrameRate; }
		static void SetFixedFrameRate(float rate) { m_FixedFrameRate = rate; }
		
		static float GetTimeScale() { return m_TimeScale; }
		static void SetTimeScale(float scale) { m_TimeScale = scale; }

	private:
		Time() = default;
		~Time() = default;

	private:
		static std::chrono::high_resolution_clock::time_point m_StartTime;

		static double m_RealTimeSinceStartup;
		static double m_DeltaTime;
		static float m_FixedFrameRate;
		static float m_TimeScale;
	};
}