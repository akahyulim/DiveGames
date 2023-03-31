#include "DivePch.h"
#include "Component.h"
#include "Scene/GameObject.h"

namespace Dive
{
	Component::Component(GameObject* pGameObject)
		: m_pGameObject(pGameObject)
	{}
}