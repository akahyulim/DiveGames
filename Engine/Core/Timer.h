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

		float GetDeltaTimeMS() const { return static_cast<float>(m_deltaTimeMS); }

	private:
		std::chrono::high_resolution_clock::time_point m_time;
		double m_deltaTimeMS = 0;
	};
}

