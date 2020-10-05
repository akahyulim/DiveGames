#include "DivePch.h"
#include "Timer.h"
#include "SystemManager.h"

namespace Dive
{
	Timer::Timer(SystemManager * pManager)
		: ISystem(pManager)
	{
		m_time = std::chrono::high_resolution_clock::now();
	}

	void Timer::Update(float deltaTime)
	{
		auto oldTime = m_time;
		m_time = std::chrono::high_resolution_clock::now();
		
		std::chrono::duration<double, std::milli> elapseTime = m_time - oldTime;

		m_deltaTimeMS = elapseTime.count();
	}
}
