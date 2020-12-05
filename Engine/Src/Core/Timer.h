#pragma once
#include "DiveDefs.h"
#include "Dive_Object.h"

namespace Dive
{
	class Dive_Context;

	constexpr double LOWEST_FRAME	= 15;
	constexpr double NO_LIMIT_FRAME = 300;
	constexpr double DELTA_FEEDBACK = 0.2;

	class Timer : public Dive_Object
	{
		NEW_DIVE_OBJECT(Timer);

	public:
		Timer(Dive_Context* context);
		~Timer();

		bool Initialize();
		void Update();

		// time
		float GetRunningTimeMS()		const { return static_cast<float>(m_runningTimeMS); }
		float GetRunningTime()			const { return static_cast<float>(m_runningTimeMS / 1000.0); }
		float GetDeltaTimeMS()			const { return static_cast<float>(m_deltaTimeMS); }
		float GetDeltaTime()			const { return static_cast<float>(m_deltaTimeMS / 1000.0); }
		float GetSmoothDeltaTimeMS()	const { return static_cast<float>(m_smoothDeltaTimeMS); }
		float GetSmoothDeltaTime()		const { return static_cast<float>(m_smoothDeltaTimeMS / 1000.0); }

		// fps
		void SetTargetFps(double target);
		float GetTargetFps()			const { return static_cast<float>(m_targetFPS); }


	private:
		std::chrono::high_resolution_clock::time_point m_startupTime;
		std::chrono::high_resolution_clock::time_point m_beginFrameTime;
		std::chrono::high_resolution_clock::time_point m_endFrameTime;
		
		double m_runningTimeMS		= 0;
		double m_deltaTimeMS		= 0;
		double m_smoothDeltaTimeMS	= 0;

		double m_targetFPS			= NO_LIMIT_FRAME;
	};
}

