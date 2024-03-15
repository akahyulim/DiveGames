#include "DivePch.h"
#include "Component.h"
#include "Transform.h"
#include "Core/CoreDefs.h"
#include "Scene/GameObject.h"

namespace Dive
{
	Component::Component(GameObject* pGameObject)
		: m_pGameObject(pGameObject)
		, m_pTransform(nullptr)
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

	Transform* Component::GetTransform()
	{
		DV_CORE_ASSERT(m_pGameObject);

		if (!m_pTransform)
			m_pTransform = m_pGameObject->GetTransform();

		return m_pTransform;
	}
}