#include "divepch.h"
#include "Timer.h"
#include "Context.h"
#include "IO/Log.h"
#include "Core/EventSystem.h"
#include "Engine/EngineEvents.h"
#include "Variant.h"

namespace Dive
{
	Timer::Timer()
	{
		Reset();
	}

	double Timer::GetMSec(bool bReset)
	{
		auto current = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = current - m_Start;

		if (bReset)
			m_Start = current;

		return elapsed.count();
	}

	void Timer::Reset()
	{
		m_Start = std::chrono::high_resolution_clock::now();
	}

	Time::Time(Context* pContext)
		: Object(pContext),
		m_FrameCount(0),
		m_DeltaTime(0.0f)
	{
		m_Time.Reset();
	}

	Time::~Time()
	{
		DV_LOG_ENGINE_TRACE("Time �Ҹ��� ȣ��");
	}
	
	void Time::BeginFrame(float deltaTime)
	{
		++m_FrameCount;
		
		if (!m_FrameCount)
			++m_FrameCount;

		m_DeltaTime = deltaTime;

		// �̺�Ʈ�� frame number�� time step �� ���� �����ؾ� �Ѵ�.

		//DV_LOG_ENGINE_DEBUG("frame: {0:d}, delta time: {1:f}", m_FrameCount, m_DeltaTime);
	}

	void Time::EndFrame()
	{
		FIRE_EVENT(EndFrameEvent());
	}

	unsigned int Time::GetTime()
	{
		return static_cast<unsigned int>(m_Time.GetMSec(false) / 1000.0f);
	}

	// ����� ���۽� m_TimeSinceLeveltLoad�� �����ؾ� �Ѵ�.
	// ����μ��� �̺�Ʈ�� ���� ������ ���δ�.
	unsigned int Time::GetTimeSinceLevelLoad()
	{
		return static_cast<unsigned int>(m_TimeSinceLevelLoad.GetMSec(false) * 0.0001f);
	}
}