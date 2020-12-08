#pragma once
#include "EventSystem.h"

namespace Dive
{
	class E_UPDATE :public IEvent
	{
	public:
		E_UPDATE(float deltaTime) : m_deltaTime(deltaTime) {}

		float GetDeltaTime() const { return m_deltaTime; }

	private:
		float m_deltaTime;
	};
}
