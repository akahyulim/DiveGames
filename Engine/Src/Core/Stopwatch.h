/*
	Stopwatch �����

	����: ������( akahyulim@gmail.com )
*/
#pragma once

namespace Dive  
{
	class Stopwatch
	{
	public:
		Stopwatch() = default;
		~Stopwatch() = default;

		void Start();

		float GetElapsedTime();
		float GetElapsedTimeMS();

	private:
		std::chrono::high_resolution_clock::time_point m_start_time;
	};
}