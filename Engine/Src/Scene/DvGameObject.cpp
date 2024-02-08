#include "divepch.h"
#include "DvGameObject.h"
#include "Core/CoreDefs.h"
#include "Components/DvTransform.h"

namespace Dive
{
	DvGameObject::DvGameObject(DvScene* pScene, const std::string& name)
		: Object(name)
		, m_pScene(pScene)
		, m_bActive(true)
		, m_bMarkedTarget(false)
	{
		m_pTransform = AddComponent<DvTransform>();
	}

	DvGameObject::~DvGameObject()
	{
		if (!m_Components.empty())
		{
			for (auto* pComponent : m_Components)
			{
				m_pScene->DeregisterComponentByID(pComponent->GetID());
				DV_DELETE(pComponent);
			}
			m_Components.clear();
			m_Components.shrink_to_fit();
		}
	}

	void DvGameObject::Update()
	{
		if (!m_bActive)
			return;

		for (auto* pComponent : m_Components)
			pComponent->Update();
	}
}