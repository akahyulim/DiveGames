#pragma once
#include "Core/DvObject.h"

namespace Dive
{
	// timer
	class DvTimer
	{
	public:
	private:
	private:
	};

	class DvContext;

	// time
	class DvTime : public DvObject
	{
		DIVE_OBJECT(DvTime, DvObject)

	public:
		explicit DvTime(DvContext* pContext);
		~DvTime();

		void BeginFrame(float timeStep);
		void EndFrame();

		unsigned int GetFrameNumber() const { return m_FrameNumber; }
		float GetTimeStep() const { return m_TimeStep; }
		unsigned int GetTimerPeriod() const { return m_TimerPeriod; }

		static unsigned int GetSystemTime();

	private:
	private:
		DvTimer m_ElapsedTime;

		unsigned int m_FrameNumber;

		float m_TimeStep;

		unsigned int m_TimerPeriod;
	};
}