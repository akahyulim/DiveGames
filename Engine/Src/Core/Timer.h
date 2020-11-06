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
		Timer(SystemManager* pManager);
		~Timer() = default;

		bool Initialize() override;
		void Update() override;

		// time
		float GetRunningTimeMS() const { return static_cast<float>(m_RunningTimeMS); }
		float GetRunningTime() const { return static_cast<float>(m_RunningTimeMS / 1000.0); }
		float GetDeltaTimeMS() const { return static_cast<float>(m_DeltaTimeMS); }
		float GetDeltaTime() const { return static_cast<float>(m_DeltaTimeMS / 1000.0); }
		float GetSmoothDeltaTimeMS() const { return static_cast<float>(m_SmoothDeltaTimeMS); }
		float GetSmoothDeltaTime() const { return static_cast<float>(m_SmoothDeltaTimeMS / 1000.0); }

		// fps
		void SetTargetFps(double target);
		float GetTargetFps() const { return static_cast<float>(m_TargetFPS); }


	private:
		std::chrono::high_resolution_clock::time_point m_StartupTime;
		std::chrono::high_resolution_clock::time_point m_BeginFrameTime;
		std::chrono::high_resolution_clock::time_point m_EndFrameTime;
		
		double m_RunningTimeMS		= 0;
		double m_DeltaTimeMS		= 0;
		double m_SmoothDeltaTimeMS	= 0;

		double m_TargetFPS			= NO_LIMIT_FRAME;
	};
}

