#include "DivePch.h"
#include "Component.h"
#include "Core/CoreDefs.h"
#include "Scene/GameObject.h"

namespace Dive
{
	Component::Component(GameObject* pGameObject)
		: m_pGameObject(pGameObject)
	{
		DV_CORE_ASSERT(pGameObject);
	}

	std::string Component::GetName() const
	{
		return m_pGameObject->GetName();
	}

	GameObject* Component::GetGameObject() const
	{
		return m_pGameObject;
	}
}