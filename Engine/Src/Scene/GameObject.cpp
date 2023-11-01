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

	bool GameObject::LoadFromFile(const std::string& name)
	{
		return false;
	}
	
	bool GameObject::SaveToFile(const std::string& name)
	{
		// game object 직렬화
		DV_CORE_INFO("Save GameObject: {0:s}, {1:d}", GetName(), GetID());

		// component 직렬화
		if (m_pTransform)
			m_pTransform->SaveToFile(name);

		// 자식들 직렬화
		for (Transform* pChild : m_pTransform->GetChildren())
			pChild->GetGameObject()->SaveToFile(name);

		return true;
	}
}