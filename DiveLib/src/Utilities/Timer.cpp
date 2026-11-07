#include "Utilities/Timer.h"

namespace Dive
{
	Timer::Timer()
	{
		m_lastTickTime = std::chrono::steady_clock::now();
	}

    void Timer::Tick()
    {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = currentTime - m_lastTickTime;
        m_lastTickTime = currentTime;

        m_deltaTime = elapsed.count();

        if (m_deltaTime > 0.1f)
        {
            m_deltaTime = 0.1f;
        }

        m_elapsedTime += m_deltaTime;

        m_frameCount++;
        m_fpsTimer += m_deltaTime;
        if (m_fpsTimer >= 1.0f)
        {
            m_fps = static_cast<uint16_t>(m_frameCount / m_fpsTimer);
            m_frameCount = 0;
            m_fpsTimer = 0.0f;
        }
    }
}