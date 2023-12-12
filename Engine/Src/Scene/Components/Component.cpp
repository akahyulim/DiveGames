#include "DivePch.h"
#include "Component.h"
#include "Transform.h"
#include "Core/CoreDefs.h"
#include "Scene/GameObject.h"

namespace Dive
{
	Component::Component(GameObject* pGameObject)
		: m_ID(0)
		, m_pGameObject(pGameObject)
		, m_pTransform(nullptr)
	{
		DV_ASSERT(pGameObject);
		m_pTransform = m_pGameObject->GetTransform();
	}

	std::string Component::GetName() const
	{
		return m_pGameObject->GetName();
	}
}