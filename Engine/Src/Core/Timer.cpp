#include "DivePch.h"
#include "Timer.h"
#include "Context.h"
#include "Log.h"

namespace Dive
{
	Timer::Timer(Context* context)
		: Subsystem(context)
	{
	}

	Timer::~Timer()
	{
		CORE_TRACE("Call Timer's Destructor =========================");
	}

	bool Timer::Initialize()
	{
		m_startupTime = std::chrono::high_resolution_clock::now();
		m_beginFrameTime = std::chrono::high_resolution_clock::now();

		return true;
	}

	void Timer::Update()
	{
		m_endFrameTime = m_beginFrameTime;
		m_beginFrameTime = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double, std::milli> elapsedTime = m_beginFrameTime - m_startupTime;
		std::chrono::duration<double, std::milli> deltaTime = m_beginFrameTime - m_endFrameTime;
		const std::chrono::duration<double, std::milli> remainTime =
			std::chrono::duration<double, std::milli>(1000.0 / m_targetFPS) - deltaTime;

		if (remainTime.count() > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int64_t>(remainTime.count())));

			elapsedTime += remainTime;
			deltaTime += remainTime;
		}
		
		m_runningTimeMS = static_cast<double>(elapsedTime.count());
		m_deltaTimeMS = static_cast<double>(deltaTime.count());

		// smooth_delta_time °è»ê
		const double lowestFrameMS = 1000.0 / LOWEST_FRAME;
		const double GapFrameMS = m_deltaTimeMS > lowestFrameMS ? lowestFrameMS : m_deltaTimeMS;
		m_smoothDeltaTimeMS = m_smoothDeltaTimeMS * (1.0 - DELTA_FEEDBACK) + GapFrameMS * DELTA_FEEDBACK;
	}

	void Timer::SetTargetFps(double target)
	{
		if (target < 0)
			m_targetFPS = NO_LIMIT_FRAME;
		else
			m_targetFPS = target;
	}
}
