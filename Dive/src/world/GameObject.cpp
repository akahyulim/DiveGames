#include "stdafx.h"
#include "GameObject.h"
#include "World.h"
#include "components/Transform.h"

namespace Dive
{
	GameObject::GameObject(const std::string& name)
		: Object(name)
		, m_world(WorldManager::GetActiveWorld())
	{
		DV_LOG(GameObject, info, "생성: {}, {}", GetName(), GetInstanceID());
	}

	GameObject::~GameObject()
	{
		m_components.clear();

		DV_LOG(GameObject, info, "소멸: {}, {}", GetName(), GetInstanceID());
	}

	void GameObject::Destory()
	{
		if (m_isDestroyed) return;
		m_isDestroyed = true;
	
		// 얕은 복사
		auto children = GetTransform()->GetChildren();
		for (auto child : children)
		{
			if (auto childGO = child->GetGameObject())
				childGO->Destory();
		}

		if (GetTransform()->HasParent())
		{ 
			auto& sibling = GetTransform()->GetParent()->GetChildren();
			auto it = std::find(sibling.begin(), sibling.end(), GetTransform());
			if (it != sibling.end())
				sibling.erase(it);
		}
		else
			m_world->DetachRoot(this);

		m_world->QueueDestroy(this);
	}

	void GameObject::Update()
	{
		if (!m_ActiveHierarchy || m_isDestroyed)
			return;

		for (auto& [type, component] : m_components)
		{
			component->Update();
		}
	}

	void GameObject::RemoveComponentByType(eComponentType type)
	{
		if (HasComponentByType(type))
		{
			auto it = m_components.find(type);
			if (it != m_components.end())
			{
				it->second.reset();
				m_components.erase(it);
			}
		}
	}

	bool GameObject::HasComponentByType(eComponentType type) const
	{
		return m_components.find(type) != m_components.end();
	}

	Component* GameObject::GetComponentByType(eComponentType type) const
	{
		auto it = m_components.find(type);
		if (it != m_components.end())
			return it->second.get();

		return nullptr;
	}

	Transform* GameObject::GetTransform() const
	{
		return GetComponent<Transform>();
	}
	
	void GameObject::SetActive(bool value)
	{
		m_activeSelf = value;
		bool parentHierarchy = GetTransform()->HasParent() ? 
			GetTransform()->GetParent()->GetGameObject()->m_ActiveHierarchy : true;

		updateActiveInHierarchy(parentHierarchy);
	}

	void GameObject::updateActiveInHierarchy(bool parentHierarchy)
	{
		m_ActiveHierarchy = parentHierarchy && m_activeSelf;

		const auto& children = GetTransform()->GetChildren();
		for (auto child : children)
		{
			auto owner = child->GetGameObject();
			owner->updateActiveInHierarchy(m_ActiveHierarchy);
		}
	}
}
