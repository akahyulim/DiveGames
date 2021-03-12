#include "Profiler.h"

namespace Dive
{
	Profiler::~Profiler()
	{
		// clear
	}

	void Profiler::SetEnabled(bool use)
	{
		if (m_bEnabled)
		{
			// 초기화 false
			// 기존 데이터 clear
			m_bEnabled = use;
		}
	}
}