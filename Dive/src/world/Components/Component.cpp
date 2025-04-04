#include "stdafx.h"
#include "Component.h"
#include "Core/CoreDefs.h"
#include "World/GameObject.h"

namespace Dive
{
	Component::Component(GameObject* gameObject)
		: m_GameObject(gameObject)
	{
	}

	std::string Component::GetName() const
	{
		return m_GameObject->GetName();
	}

	GameObject* Component::GetGameObject() const
	{
		return m_GameObject;
	}
}
