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
			// �ʱ�ȭ false
			// ���� ������ clear
			m_bEnabled = use;
		}
	}
}