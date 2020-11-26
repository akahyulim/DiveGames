#include "DivePch.h"
#include "Timer.h"
#include "SystemManager.h"
#include "Log.h"


namespace Dive
{
	Timer::Timer(const std::shared_ptr<SystemManager>& manager)
		: System(manager)
	{
	}

	Timer::~Timer()
	{
		CORE_TRACE("Call Timer's Destructor =========================");
	}

	bool Timer::Initialize()
	{
		m_StartupTime = std::chrono::high_resolution_clock::now();
		m_BeginFrameTime = std::chrono::high_resolution_clock::now();

		return true;
	}

	void Timer::Update()
	{
		m_EndFrameTime = m_BeginFrameTime;
		m_BeginFrameTime = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double, std::milli> elapsedTime = m_BeginFrameTime - m_StartupTime;
		std::chrono::duration<double, std::milli> deltaTime = m_BeginFrameTime - m_EndFrameTime;
		const std::chrono::duration<double, std::milli> remainTime =
			std::chrono::duration<double, std::milli>(1000.0 / m_TargetFPS) - deltaTime;

		if (remainTime.count() > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int64_t>(remainTime.count())));

			elapsedTime += remainTime;
			deltaTime += remainTime;
		}
		
		m_RunningTimeMS = static_cast<double>(elapsedTime.count());
		m_DeltaTimeMS = static_cast<double>(deltaTime.count());

		// smooth_delta_time °è»ê
		const double lowestFrameMS = 1000.0 / LOWEST_FRAME;
		const double GapFrameMS = m_DeltaTimeMS > lowestFrameMS ? lowestFrameMS : m_DeltaTimeMS;
		m_SmoothDeltaTimeMS = m_SmoothDeltaTimeMS * (1.0 - DELTA_FEEDBACK) + GapFrameMS * DELTA_FEEDBACK;
	}

	void Timer::SetTargetFps(double target)
	{
		if (target < 0)
			m_TargetFPS = NO_LIMIT_FRAME;
		else
			m_TargetFPS = target;
	}
}
