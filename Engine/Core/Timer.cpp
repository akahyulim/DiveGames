#include "DivePch.h"
#include "Timer.h"
#include "SystemManager.h"

namespace Dive
{
	Timer::Timer(SystemManager * pManager)
		: ISystem(pManager)
	{
		m_startup_time = std::chrono::high_resolution_clock::now();
		m_begin_frame_time = std::chrono::high_resolution_clock::now();
	}

	void Timer::Update(float deltaTime)
	{
		m_end_frame_time = m_begin_frame_time;
		m_begin_frame_time = std::chrono::high_resolution_clock::now();

		const std::chrono::duration<double, std::milli> elapsed_time = m_begin_frame_time - m_startup_time;
		std::chrono::duration<double, std::milli> delta_time = m_begin_frame_time - m_end_frame_time;
		const std::chrono::duration<double, std::milli> remain_time =
			std::chrono::duration<double, std::milli>(1000.0 / m_target_fps) - delta_time;

		if (remain_time.count() > 0)
		{

		}

		m_running_time_ms = static_cast<double>(elapsed_time.count());
		m_delta_time_ms = static_cast<double>(delta_time.count());
	}
}
