#pragma once
#include "Core/Object.h"

namespace Dive
{
	// timer
	class Timer
	{
	public:
		Timer();
		~Timer() = default;

		double GetMSec(bool bReset);

		void Reset();

	private:
		std::chrono::high_resolution_clock::time_point m_Start;
	};

	class Context;

	// time
	class Time : public Object
	{
		DIVE_OBJECT(Time, Object)

	public:
		explicit Time(Context* pContext);
		~Time();

		// 프레임 시작.
		void BeginFrame(float deltaTime);
		// 프레임 종료.
		void EndFrame();

		// 프레임 개수.
		unsigned int GetFrameCount() const { return m_FrameCount; }
		// 프레임 간 경과 시간.
		float GetDeltaTime() const { return m_DeltaTime; }

		// 초단위 프로그램 실행 경과 시간.
		unsigned int GetTime();
		// 초단위 씬 실행 경과 시간.
		unsigned int GetTimeSinceLevelLoad();

	private:
	private:
		// 프로그램 실행 시간 타이머.
		Timer m_Time;
		// 씬 실행 시간 타이머.
		Timer m_TimeSinceLevelLoad;

		// 프레임 개수.
		unsigned int m_FrameCount;

		// 프레임 간격 시간.
		float m_DeltaTime;
	};
}