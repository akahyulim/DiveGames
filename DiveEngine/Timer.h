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
		std::chrono::high_resolution_clock::time_point mStartTime;
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

		double GetDeltaTimeMS() const { return mDeltaTime; }
		float GetDeltaTimeSec() const { return static_cast<float>(mDeltaTime / 1000.0f); }
 
		double GetRealTimeSinceStartUpMS() const { return mRealTimeSinceStartup; }
		unsigned int GetRealTimeSinceStartUpSec() const { return static_cast<unsigned int>(mRealTimeSinceStartup / 1000); }

		unsigned long long GetFrameCount() const { return mFrameCount; }

		float GetFixedFrameRate() const { return mFixedFrameRate; }
		void SetFixedFrameRate(float rate) { mFixedFrameRate = rate; }
		
		// 렌더링 구현 후 fps 출력과 함께 테스트 해보기
		float GetTimeScale() const { return mTimeScale; }
		void SetTimeScale(float scale) { mTimeScale = scale; }

	private:
		TimeManager() = default;
		~TimeManager() = default;

	private:
		std::chrono::high_resolution_clock::time_point mStartTime;

		double mRealTimeSinceStartup = 0;
		double mDeltaTime = 0;
		float mFixedFrameRate = 0.02f;
		float mTimeScale = 1.0f;

		unsigned long long mFrameCount = 0;
	};
}