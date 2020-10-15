/*
	Stopwatch ������

	����: ������( akahyulim@gmail.com )
*/

//= INCLUDES ====================
#include "DivePch.h"
#include "Stopwatch.h"
//===============================

namespace Dive 
{
	void Stopwatch::Start()
	{
		m_start_time = std::chrono::high_resolution_clock::now();
	}

	float Stopwatch::GetElapsedTime()
	{
		std::chrono::duration<double, std::milli> ms = std::chrono::high_resolution_clock::now() - m_start_time;

		return static_cast<float>(ms.count() / 1000.0f);
	}

	float Stopwatch::GetElapsedTimeMS()
	{
		std::chrono::duration<double, std::milli> ms = std::chrono::high_resolution_clock::now() - m_start_time;

		return static_cast<float>(ms.count());
	}
}