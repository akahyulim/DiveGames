#include "divepch.h"
#include "Component.h"
#include "Scene/GameObject.h"

namespace Dive
{
	unsigned long long Component::GetInstanceID() const
	{
		if (!m_pGameObject)
			return 0;

		return m_pGameObject->GetInstanceID();
	}
}