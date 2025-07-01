#include "stdafx.h"
#include "GameObject.h"
#include "World.h"
#include "components/Transform.h"

namespace Dive
{
	GameObject::GameObject(const std::string& name)
		: Object(name)
		, m_World(WorldManager::GetActiveWorld())
	{
		DV_LOG(GameObject, trace, "Created: {}, {}", GetName(), GetInstanceID());
	}

	GameObject::~GameObject()
	{
		m_Components.clear();

		DV_LOG(GameObject, trace, "Destroyed: {}, {}", GetName(), GetInstanceID());
	}

	void GameObject::Destory()
	{
		if (m_IsDestroyed) return;
		m_IsDestroyed = true;
	
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
			m_World->DetachRoot(this);

		m_World->QueueDestroy(this);
	}

	void GameObject::Update()
	{
		if (!m_ActiveHierarchy || m_IsDestroyed)
			return;

		for (auto& [type, component] : m_Components)
		{
			component->Update();
		}
	}

	void GameObject::RemoveComponentByType(eComponentType type)
	{
		if (!HasComponentByType(type))
		{
			DV_LOG(GameObject, warn, "Component of type hash {} does not exist on GameObject '{}'.", (int)type, GetName());
			return;
		}
		auto it = m_Components.find(type);
		if (it != m_Components.end())
		{
			it->second.reset();
			m_Components.erase(it);
		}
		else
		{
			DV_LOG(GameObject, warn, "Failed to remove component of type hash {} from GameObject '{}'.", (int)type, GetName());
		}
	}

	bool GameObject::HasComponentByType(eComponentType type) const
	{
		return m_Components.find(type) != m_Components.end();
	}

	Component* GameObject::GetComponentByType(eComponentType type) const
	{
		auto it = m_Components.find(type);
		if (it != m_Components.end())
			return it->second.get();

		return nullptr;
	}

	Transform* GameObject::GetTransform() const
	{
		return GetComponent<Transform>();
	}
	
	void GameObject::SetActive(bool value)
	{
		m_ActiveSelf = value;
		bool parentHierarchy = GetTransform()->HasParent() ? 
			GetTransform()->GetParent()->GetGameObject()->m_ActiveHierarchy : true;

		updateActiveInHierarchy(parentHierarchy);
	}

	void GameObject::updateActiveInHierarchy(bool parentHierarchy)
	{
		m_ActiveHierarchy = parentHierarchy && m_ActiveSelf;

		const auto& children = GetTransform()->GetChildren();
		for (auto child : children)
		{
			auto owner = child->GetGameObject();
			owner->updateActiveInHierarchy(m_ActiveHierarchy);
		}
	}
}