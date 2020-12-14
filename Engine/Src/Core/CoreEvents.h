#pragma once
#include "EventSystem.h"

namespace Dive
{
	class E_UPDATE :public IEvent
	{
	public:
		E_UPDATE(float elapsedTimeSec, float deltaTimeSec)
			: m_elapsedTimeSec(elapsedTimeSec),
			m_deltaTimeSec(deltaTimeSec)
		{}

		float GetElapsedTimeSec()	const { return m_elapsedTimeSec; }
		float GetDeltaTimeSec()		const { return m_deltaTimeSec; }

	private:
		float m_elapsedTimeSec;
		float m_deltaTimeSec;
	};
}
