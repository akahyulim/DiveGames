#include "stdafx.h"
#include "Component.h"
#include "../GameObject.h"

namespace Dive
{
	Component::Component(GameObject* gameObject)
		: m_gameObject(gameObject)
	{
	}

	std::string Component::GetName() const
	{
		assert(m_gameObject);
		return m_gameObject->GetName();
	}

	GameObject* Component::GetGameObject() const
	{
		assert(m_gameObject);
		return m_gameObject;
	}

	Transform* Component::GetTransform() const
	{
		assert(m_gameObject);
		return m_gameObject->GetTransform();
	}
}
