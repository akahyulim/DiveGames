#include "Timer.h"

using namespace std;


namespace dive
{
	Timer::Timer()
	{
		Reset();
	}

	void Timer::Reset()
	{
		mStartTime = chrono::high_resolution_clock::now();
	}

	double Timer::GetElapsedTime(bool reset)
	{
		auto currentTime = chrono::high_resolution_clock::now();
		chrono::duration<double, std::milli> elapsedTime = currentTime - mStartTime;

		if (reset)
		{
			mStartTime = currentTime;
		}

		return elapsedTime.count();
	}

	void TimeManager::Initialize()
	{
		mStartTime = chrono::high_resolution_clock::now();
	}

	void TimeManager::Update()
	{
		auto currentTime = chrono::high_resolution_clock::now();
		chrono::duration<double, std::milli> elapsedTime = currentTime - mStartTime;

		mRealTimeSinceStartup += elapsedTime.count();
		mDeltaTime = elapsedTime.count() * mTimeScale;

		mStartTime = currentTime;

		++mFrameCount;
	}
}