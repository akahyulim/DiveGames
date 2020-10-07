#pragma once
#include "DiveDefs.h"
#include "ISystem.h"

namespace Dive
{
	class SystemManager;

	constexpr double LOWEST_FRAME	= 15;
	constexpr double NO_LIMIT_FRAME = 300;
	constexpr double DELTA_FEEDBACK = 0.2;

	class Timer : public ISystem
	{
	public:
		Timer(SystemManager* manager);
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


	private:
		std::chrono::high_resolution_clock::time_point m_startup_time;
		std::chrono::high_resolution_clock::time_point m_begin_frame_time;
		std::chrono::high_resolution_clock::time_point m_end_frame_time;
		
		double m_running_time_ms		= 0;
		double m_delta_time_ms			= 0;
		double m_smooth_delta_time_ms	= 0;

		double m_target_fps				= NO_LIMIT_FRAME;
	};
}

