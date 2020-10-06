#pragma once
#include "Isystem.h"

namespace Dive
{
	class SystemManager;

	class Timer : public ISystem
	{
	public:
		Timer(SystemManager* pManager);
		~Timer() = default;

		void Update(float deltaTime) override;

		// time
		float GetRunningTimeMS() const { return static_cast<float>(m_running_time_ms); }
		float GetRunningTime() const { return static_cast<float>(m_running_time_ms / 1000.0); }
		float GetDeltaTimeMS() const { return static_cast<float>(m_delta_time_ms); }
		float GetDeltaTime() const { return static_cast<float>(m_delta_time_ms / 1000.0); }
		float GetSmoothDeltaTimeMS() const { return static_cast<float>(m_smooth_delta_time_ms); }
		float GetSmoothDeltaTime() const { return static_cast<float>(m_smooth_delta_time_ms / 1000.0); }

		// fps
		void SetTargetFps(double target);
		float GetTargetFps() const { return static_cast<float>(m_target_fps); }
		float GetFps() const { return static_cast<float>(m_fps); }


	private:
		std::chrono::high_resolution_clock::time_point m_startup_time;
		std::chrono::high_resolution_clock::time_point m_begin_frame_time;
		std::chrono::high_resolution_clock::time_point m_end_frame_time;
		
		double m_running_time_ms		= 0;
		double m_delta_time_ms			= 0;
		double m_smooth_delta_time_ms	= 0;

		double m_fps		= 0;
		double m_target_fps	= 0;
	};
}

