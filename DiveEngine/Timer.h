#pragma once

namespace Dive
{
	class Timer
	{
	public:
		Timer();
		~Timer() = default;

		static void Start();

		static double GetTotalTime();
		double GetElapsedTime();

		void Record();

	private:
		double m_lastTime = 0;
	};
}