#include "divepch.h"
#include "Component.h"
#include "Scene/GameObject.h"

namespace Dive
{
	Component::Component(GameObject* pGameObject)
		: m_pGameObject(pGameObject)
	{
		SetName(m_pGameObject->GetName());
	}

	Component::Component(GameObject* pGameObject, unsigned long long id)
		: m_pGameObject(pGameObject), Object(id)
	{
		SetName(m_pGameObject->GetName());
	}
}