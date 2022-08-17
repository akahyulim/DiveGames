#include "divepch.h"
#include "GameObject.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	GameObject::GameObject(Context* pContext)
		: Object(pContext),
		m_pScene(nullptr),
		m_ID(0),
		m_bActive(true),
		m_bMarkedTarget(false)
	{
	}

	GameObject::~GameObject()
	{
		DV_LOG_ENGINE_DEBUG("GameObject( {0:s} : {1:d}) �Ҹ��� ȣ��", m_Name, m_ID);

		// �ϴ� �׽�Ʈ
		// ���� Scene Ȥ�� �ٸ� ���� ����� Component����
		// ���� Ȥ�� �̺�Ʈ�� ���� �����ؾ� �Ѵ�.
		if (!m_Components.empty())
		{
			for (auto* pComponent : m_Components)
			{
				if (m_pScene)
					m_pScene->ComponentRemoved(pComponent);
				
				DV_DELETE(pComponent);
			}
			m_Components.clear();
		}
	}

	void GameObject::RemoveComponent(Component* pComponent)
	{
		if (!pComponent)
			return;

		if (m_pScene)
		{
			m_pScene->ComponentRemoved(pComponent);
		}

		auto it = m_Components.begin();
		for (it; it != m_Components.end(); ++it)
		{
			if ((*it)->GetID() == pComponent->GetID())
			{
				DV_DELETE(*it);
				m_Components.erase(it);
				return;
			}
		}
	}
}