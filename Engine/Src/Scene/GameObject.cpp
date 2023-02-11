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
		AddComponent<Transform>();
	}

	GameObject::~GameObject()
	{
		if (!m_Components.empty())
		{
			for (auto* pComponent : m_Components)
			{
				if (m_pScene)
					m_pScene->DeregisterComponent(pComponent);
				
				DV_DELETE(pComponent);
			}
			m_Components.clear();
		}

		DV_LOG_ENGINE_TRACE("GameObject ¼Ò¸ê ¿Ï·á({0:d} : {1:s})", m_ID, m_Name);
	}

	void GameObject::Update(float delta)
	{
		if (!m_bActive)
			return;

		for (auto pComponent : m_Components)
		{
			pComponent->Update(delta);
		}
	}

	void GameObject::RemoveComponent(Component* pComponent)
	{
		if (!pComponent)
			return;

		if (m_pScene)
			m_pScene->DeregisterComponent(pComponent);

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