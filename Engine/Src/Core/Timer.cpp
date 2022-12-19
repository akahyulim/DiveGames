#include "divepch.h"
#include "Timer.h"
#include "Context.h"
#include "IO/Log.h"
#include "Core/EventSystem.h"
#include "Engine/EngineEvents.h"

namespace Dive
{
	Timer::Timer()
	{
		Reset();
	}

	uint32_t Timer::GetMSec(bool bReset)
	{
		auto current = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = current - m_Start;

		if (bReset)
			m_Start = current;

		return static_cast<uint32_t>(elapsed.count());
	}

	void Timer::Reset()
	{
		m_Start = std::chrono::high_resolution_clock::now();
	}

	Time::Time(Context* pContext)
		: Object(pContext),
		m_FrameCount(0),
		m_TimeStep(0.0f)
	{
		m_ElapsedTime.Reset();
	}

	Time::~Time()
	{
		DV_LOG_ENGINE_TRACE("Time ¼Ò¸ê ¿Ï·á");
	}
	
	void Time::BeginFrame(float timeStep)
	{
		++m_FrameCount;
		if (!m_FrameCount)
			++m_FrameCount;

		m_TimeStep = timeStep;

		FIRE_EVENT(BeginFrameEvent(m_FrameCount, m_TimeStep));
	}

	void Time::EndFrame()
	{
		FIRE_EVENT(EndFrameEvent());
	}

	float Time::GetElapsedTime()
	{
		return static_cast<float>(m_ElapsedTime.GetMSec(false)) / 1000.0f;
	}

	float Time::GetFPS() const
	{
		return 1.0f / m_TimeStep;
	}

	void Time::Sleep(uint32_t milliSec)
	{
		::Sleep(static_cast<DWORD>(milliSec));
	}
}