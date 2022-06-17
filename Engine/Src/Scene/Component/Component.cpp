#include "divepch.h"
#include "Component.h"
#include "Scene/GameObject.h"

namespace Dive
{
	Component::Component(GameObject* pGameObject)
		: m_pGameObject(pGameObject)
	{
	}

	uint64_t Component::GetInstanceID() const 
	{ 
		return !m_pGameObject ? 0 : m_pGameObject->GetInstanceID(); 
	}

	std::string Component::GetName() const 
	{ 
		return !m_pGameObject ? std::string() : m_pGameObject->GetName(); 
	}
}