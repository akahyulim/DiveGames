#include "stdafx.h"
#include "Component.h"
#include "../GameObject.h"

namespace Dive
{
	Component::Component(GameObject* gameObject)
		: m_GameObject(gameObject)
	{
	}

	std::string Component::GetName() const
	{
		assert(m_GameObject);
		return m_GameObject->GetName();
	}

	GameObject* Component::GetGameObject() const
	{
		assert(m_GameObject);
		return m_GameObject;
	}
}
