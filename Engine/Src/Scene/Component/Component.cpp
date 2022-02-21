#include "divepch.h"
#include "Component.h"
#include "Scene/GameObject.h"

namespace Dive
{
	unsigned long long Component::GetInstanceID() const
	{
		DV_ASSERT(m_pGameObject != nullptr);
		return m_pGameObject->GetInstanceID();
	}
}