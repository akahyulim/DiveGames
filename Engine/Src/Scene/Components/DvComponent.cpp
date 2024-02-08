#include "DivePch.h"
#include "DvComponent.h"
#include "DvTransform.h"
#include "Core/CoreDefs.h"
#include "Scene/DvGameObject.h"

namespace Dive
{
	DvComponent::DvComponent(DvGameObject* pGameObject)
		: m_pGameObject(pGameObject)
		, m_pTransform(nullptr)
	{
		DV_CORE_ASSERT(pGameObject);
	}

	std::string DvComponent::GetName() const
	{
		return m_pGameObject->GetName();
	}

	DvGameObject* DvComponent::GetGameObject() const
	{
		return m_pGameObject;
	}

	DvTransform* DvComponent::GetTransform()
	{
		if (!m_pTransform)
			m_pTransform = m_pGameObject->GetTransform();

		DV_CORE_ASSERT(m_pTransform);
		return m_pTransform;
	}
}