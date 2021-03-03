#pragma once

namespace Dive
{
	class Dive_Timer
	{
	public:
		Dive_Timer();
		~Dive_Timer() = default;

		static void Start();

		static double GetTotalTime();
		double GetElapsedTime();

		void Record();

	private:
		double m_lastTime = 0;
	};
}