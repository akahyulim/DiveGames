#pragma once
#include <chrono>

namespace dive
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

		double GetDeltaTimeMS() const { return m_deltaTime; }
		float GetDeltaTimeSec() const { return static_cast<float>(m_deltaTime / 1000.0f); }
 
		double GetRealTimeSinceStartUpMS() const { return m_realTimeSinceStartup; }
		unsigned int GetRealTimeSinceStartUpSec() const { return static_cast<unsigned int>(m_realTimeSinceStartup / 1000); }

		unsigned long long GetFrameCount() const { return m_frameCount; }

		float GetFixedFrameRate() const { return m_fixedFrameRate; }
		void SetFixedFrameRate(float rate) { m_fixedFrameRate = rate; }
		
		// 렌더링 구현 후 fps 출력과 함께 테스트 해보기
		float GetTimeScale() const { return m_timeScale; }
		void SetTimeScale(float scale) { m_timeScale = scale; }

	private:
		TimeManager() = default;
		~TimeManager() = default;

	private:
		std::chrono::high_resolution_clock::time_point m_startTime;

		double m_realTimeSinceStartup = 0;
		double m_deltaTime = 0;
		float m_fixedFrameRate = 0.02f;
		float m_timeScale = 1.0f;

		unsigned long long m_frameCount = 0;
	};
}