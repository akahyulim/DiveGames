#pragma once
#include <chrono>

namespace Dive
{
	class Timer
	{
	public:
		Timer();
		~Timer() = default;

		void Reset();

		double GetElapsedTime(bool reset = false);

	private:
		std::chrono::high_resolution_clock::time_point m_startTime;
	};

	class TimeManager
	{
	public:
		static TimeManager& GetInstance()
		{
			static TimeManager instance;
			return instance;
		}
		
		void Initialize();
		void Update();

		double GetRealTimeSinceStartUpMS() const { return m_realTimeSinceStartup; }
		unsigned int GetRealTimeSinceStartUpSec() const { return static_cast<unsigned int>(m_realTimeSinceStartup / 1000); }

		unsigned long long GetFrameCount() const { return m_frameCount; }

		void SetFixedFrameRate(float rate) { m_fixedFrameRate = rate; }
		float GetFixedFrameRate() const { return m_fixedFrameRate; }

	private:
		TimeManager() = default;
		~TimeManager() = default;

	private:
		std::chrono::high_resolution_clock::time_point m_startTime;

		double m_realTimeSinceStartup = 0;
		double m_deltaTime = 0;
		float m_fixedFrameRate = 0.2f;
		float m_timeScale = 1.0f;

		unsigned long long m_frameCount = 0;
	};
}