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
		std::chrono::high_resolution_clock::time_point m_StartTime;
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

		double GetDeltaTimeMS() const { return m_DeltaTime; }
		float GetDeltaTimeSec() const { return static_cast<float>(m_DeltaTime / 1000.0f); }
 
		double GetRealTimeSinceStartUpMS() const { return m_RealTimeSinceStartup; }
		unsigned int GetRealTimeSinceStartUpSec() const { return static_cast<unsigned int>(m_RealTimeSinceStartup / 1000); }

		unsigned long long GetFrameCount() const { return m_FrameCount; }

		float GetFixedFrameRate() const { return m_FixedFrameRate; }
		void SetFixedFrameRate(float rate) { m_FixedFrameRate = rate; }
		
		// 렌더링 구현 후 fps 출력과 함께 테스트 해보기
		float GetTimeScale() const { return m_TimeScale; }
		void SetTimeScale(float scale) { m_TimeScale = scale; }

	private:
		TimeManager() = default;
		~TimeManager() = default;

	private:
		std::chrono::high_resolution_clock::time_point m_StartTime;

		double m_RealTimeSinceStartup = 0;
		double m_DeltaTime = 0;
		float m_FixedFrameRate = 0.02f;
		float m_TimeScale = 1.0f;

		unsigned long long m_FrameCount = 0;
	};
}