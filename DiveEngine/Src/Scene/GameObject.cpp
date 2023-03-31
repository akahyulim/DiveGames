#include "DivePch.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Drawable.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	GameObject::GameObject(const std::string& name)
		: m_bActive(true),
		m_bMarkedTarget(false),
		m_pTransform(nullptr),
		m_pDrawable(nullptr)
	{
		SetName(name);
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

	void GameObject::Update(float delta)
	{
		if (!m_bActive)
			return;

		for (auto* pComponent : m_Components)
			pComponent->Update(delta);
	}
}