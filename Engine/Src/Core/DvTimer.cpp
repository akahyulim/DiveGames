#include "divepch.h"
#include "DvTimer.h"
#include "DvContext.h"
#include "IO/DvLog.h"
#include "Core/DvEventSystem.h"
#include "Variant.h"

namespace Dive
{
	DvTime::DvTime(DvContext* pContext)
		: DvObject(pContext),
		m_FrameNumber(0),
		m_TimeStep(0.0f),
		m_TimerPeriod(0)
	{

	}

	DvTime::~DvTime()
	{
	}
	
	void DvTime::BeginFrame(float timeStep)
	{
		++m_FrameNumber;
		
		if (!m_FrameNumber)
			++m_FrameNumber;

		m_TimeStep = timeStep;

		// 이벤트로 frame number와 time step 두 값을 전달해야 한다.

		DV_LOG_ENGINE_DEBUG("frame: {0:d}, step: {1:f}", m_FrameNumber, m_TimeStep);
	}

	void DvTime::EndFrame()
	{
		DV_EVENT_FIRE(eDvEventType::EndFrame);
	}
	unsigned int DvTime::GetSystemTime()
	{
		return 0;
	}
}