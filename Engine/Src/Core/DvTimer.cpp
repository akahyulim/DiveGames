#include "divepch.h"
#include "DvTimer.h"
#include "DvContext.h"
#include "IO/DvLog.h"
#include "Core/DvEventSystem.h"
#include "Variant.h"

namespace Dive
{
	DvTimer::DvTimer()
	{
		Reset();
	}

	double DvTimer::GetMSec(bool bReset)
	{
		auto current = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = current - m_Start;

		if (bReset)
			m_Start = current;

		return elapsed.count();
	}

	void DvTimer::Reset()
	{
		m_Start = std::chrono::high_resolution_clock::now();
	}

	DvTime::DvTime(DvContext* pContext)
		: DvObject(pContext),
		m_FrameCount(0),
		m_DeltaTime(0.0f)
	{
		m_Time.Reset();
	}

	DvTime::~DvTime()
	{
	}
	
	void DvTime::BeginFrame(float deltaTime)
	{
		++m_FrameCount;
		
		if (!m_FrameCount)
			++m_FrameCount;

		m_DeltaTime = deltaTime;

		// 이벤트로 frame number와 time step 두 값을 전달해야 한다.

		DV_LOG_ENGINE_DEBUG("frame: {0:d}, delta time: {1:f}", m_FrameCount, m_DeltaTime);
	}

	void DvTime::EndFrame()
	{
		DV_EVENT_FIRE(eDvEventType::EndFrame);
	}

	unsigned int DvTime::GetTime()
	{
		return static_cast<unsigned int>(m_Time.GetMSec(false) / 1000.0f);
	}

	// 장면의 시작시 m_TimeSinceLeveltLoad를 리셋해야 한다.
	// 현재로서는 이벤트가 가장 적절해 보인다.
	unsigned int DvTime::GetTimeSinceLevelLoad()
	{
		return static_cast<unsigned int>(m_TimeSinceLevelLoad.GetMSec(false) * 0.0001f);
	}
}