#include "DivePch.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "Core/CoreDefs.h"
#include "Core/Log.h"

namespace Dive
{
	GameObject::GameObject(const std::string& name)
		: Object(name) 
		, m_bActive(true)
		, m_bMarkedTarget(false)
		, m_pTransform(nullptr)
	{
		m_pTransform = AddComponent<Transform>();
	}

	GameObject::~GameObject()
	{
		if (!m_Components.empty())
		{
			for (auto* pComponent : m_Components)
			{
				Scene::DeregisterComponentByID(pComponent->GetID());
				DV_DELETE(pComponent);
			}
			m_Components.clear();
			m_Components.shrink_to_fit();
		}
	}

	void GameObject::Update()
	{
		if (!m_bActive)
			return;

		for (auto* pComponent : m_Components)
			pComponent->Update();
	}
}