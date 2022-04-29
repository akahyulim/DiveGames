#include "divepch.h"
#include "Component.h"
#include "Scene/GameObject.h"

namespace Dive
{
	Component::Component(GameObject* pGameObject, unsigned long long id)
		: m_pGameObject(pGameObject), Object(pGameObject->GetName(), id)
	{
	}
}