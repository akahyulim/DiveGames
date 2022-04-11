#pragma once

namespace Dive
{
	class Time
	{
	public:
		Time() = default;
		~Time() = default;

		void Initialize();

		void Tick();

		double GetDeltaTimeMS() const { return m_DeltaTime; }
		float GetDeltaTimeSec() const { return static_cast<float>(m_DeltaTime / 1000.0f); }
 
		double GetRealTimeSinceStartUpMS() const { return m_RealTimeSinceStartup; }
		unsigned int GetRealTimeSinceStartUpSec() const { return static_cast<unsigned int>(m_RealTimeSinceStartup / 1000); }

		float GetFixedFrameRate() const { return m_FixedFrameRate; }
		void SetFixedFrameRate(float rate) { m_FixedFrameRate = rate; }
		
		float GetTimeScale() const { return m_TimeScale; }
		void SetTimeScale(float scale) { m_TimeScale = scale; }

	private:
		Time(const Time&) = delete;
		Time& operator=(const Time&) = delete;

	private:
		std::chrono::high_resolution_clock::time_point m_StartTime;

		double m_RealTimeSinceStartup	= 0;
		double m_DeltaTime				= 0;
		float m_FixedFrameRate			= 0.0f;
		float m_TimeScale				= 0.0f;
	};
}