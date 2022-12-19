#pragma once
#include "Core/Object.h"

namespace Dive
{
	// 1/1000초 단위의 경과시간을 측정하는 타이머 클래스. 
	class Timer
	{
	public:
		Timer();
		~Timer() = default;

		uint32_t GetMSec(bool bReset);

		void Reset();

	private:
		std::chrono::high_resolution_clock::time_point m_Start;
	};

	class Context;

	// 시스템 시간 클래스.
	class Time : public Object
	{
		DIVE_OBJECT(Time, Object)

	public:
		explicit Time(Context* pContext);
		~Time() override;

		void BeginFrame(float timeStep);
		void EndFrame();

		uint32_t GetFrameCount() const { return m_FrameCount; }
		
		float GetElapsedTime();

		float GetFPS() const;

		static void Sleep(uint32_t milliSec);

	private:
		Timer m_ElapsedTime;
		uint32_t m_FrameCount;
		float m_TimeStep;
	};
}