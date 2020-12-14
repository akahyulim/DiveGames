#include "DivePch.h"
#include "Time.h"
#include "Context.h"
#include "EventSystem.h"
#include "CoreEvents.h"
#include "Log.h"


namespace Dive
{
	Timer::Timer()
	{
		Reset();
	}

	double Timer::GetElapsedTime(bool reset)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsedTime = currentTime - m_startTime;

		if (reset)
			m_startTime = currentTime;

		return elapsedTime.count();
	}

	void Timer::Reset()
	{
		m_startTime = std::chrono::high_resolution_clock::now();
	}

	Time::Time(Context* context)
		: Object(context)
	{
		m_startTime = std::chrono::high_resolution_clock::now();
		m_frameTime = m_startTime;
	}

	Time::~Time()
	{
		CORE_TRACE("Call Time's Destructor =========================");
	}

	void Time::Update()
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed	= currentTime - m_startTime;
		std::chrono::duration<double, std::milli> delta		= currentTime - m_frameTime;

		m_elapsedTimeMS = elapsed.count();
		m_deltaTimeMS	= delta.count();
		
		m_frameTime = currentTime;

		E_UPDATE evnt(GetElapsedTimeSec(), GetDeltaTimeSec());
		DIVE_FIRE_EVENT(&evnt);
	}
}
