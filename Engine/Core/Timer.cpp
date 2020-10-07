#include "DivePch.h"
#include "Timer.h"
#include "SystemManager.h"

namespace Dive
{
	Timer::Timer(SystemManager * manager)
		: ISystem(manager)
	{
		m_startup_time = std::chrono::high_resolution_clock::now();
		m_begin_frame_time = std::chrono::high_resolution_clock::now();
	}

	void Timer::Update(float deltaTime)
	{
		m_end_frame_time = m_begin_frame_time;
		m_begin_frame_time = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double, std::milli> elapsed_time = m_begin_frame_time - m_startup_time;
		std::chrono::duration<double, std::milli> delta_time = m_begin_frame_time - m_end_frame_time;
		const std::chrono::duration<double, std::milli> remain_time =
			std::chrono::duration<double, std::milli>(1000.0 / m_target_fps) - delta_time;

		if (remain_time.count() > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int64_t>(remain_time.count())));

			elapsed_time += remain_time;
			delta_time += remain_time;
		}
		
		m_running_time_ms = static_cast<double>(elapsed_time.count());
		m_delta_time_ms = static_cast<double>(delta_time.count());

		// smooth_delta_time °è»ê
		const double lowest_frame_ms = 1000.0 / LOWEST_FRAME;
		const double gap_frame_ms = m_delta_time_ms > lowest_frame_ms ? lowest_frame_ms : m_delta_time_ms;
		m_smooth_delta_time_ms = m_smooth_delta_time_ms * (1.0 - DELTA_FEEDBACK) + gap_frame_ms * DELTA_FEEDBACK;
	}

	void Timer::SetTargetFps(double target)
	{
		if (target < 0)
			m_target_fps = NO_LIMIT_FRAME;
		else
			m_target_fps = target;
	}
}
