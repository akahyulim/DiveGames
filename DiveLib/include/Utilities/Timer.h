#pragma once
#include <cstdint>
#include <chrono>

namespace Dive
{
	class Timer
	{
	public:
		Timer();
		~Timer() = default;

		 void Tick();

		 float GetDeltaTime() { return m_deltaTime; }
		 double GetElapsedTime() { return m_elapsedTime; }
		 uint16_t GetFps() const { return m_fps; }

	private:
		 std::chrono::steady_clock::time_point m_lastTickTime;
		 float m_deltaTime;
		 double m_elapsedTime;

		 uint32_t m_frameCount = 0;
		 float m_fpsTimer = 0.0f;
		 uint16_t m_fps;
	};
}