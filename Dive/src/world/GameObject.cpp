#include "stdafx.h"
#include "GameObject.h"
#include "components/Transform.h"
#include "core/instanceID.h"

namespace Dive
{
	GameObject::GameObject(const std::string& name)
		: m_InstanceID(InstanceID())
		, m_Name(name)
		, m_Transform(AddComponent<Transform>())
	{
		DV_LOG(GameObject, trace, "GameObject '{}' with InstanceID {} is being created.", GetName(), GetInstanceID());
	}

	GameObject::GameObject(UINT64 instanceID, const std::string& name)
		: m_InstanceID(instanceID)
		, m_Name(name)
		,m_Transform(AddComponent<Transform>())
	{
		DV_LOG(GameObject, trace, "GameObject '{}' with InstanceID {} is being created.", GetName(), GetInstanceID());
	}

	GameObject::~GameObject()
	{
		DV_LOG(GameObject, trace, "GameObject '{}' with InstanceID {} is being destroyed.", GetName(), GetInstanceID());

		for (auto& [type, component] : m_Components)
		{
			DV_DELETE(component);
		}
		m_Components.clear();
	}

	void GameObject::Update()
	{
		if (!m_ActiveHierarchy)
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
			DV_DELETE(it->second);
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
		if(!HasComponentByType(type))
			return nullptr;

		return m_Components.at(type);
	}

	void GameObject::SetActive(bool value)
	{
		m_ActiveSelf = value;
		bool parentHierarchy = m_Transform->HasParent() ? 
			m_Transform->GetParent()->GetGameObject()->m_ActiveHierarchy : true;

		updateActiveInHierarchy(parentHierarchy);
	}

	void GameObject::updateActiveInHierarchy(bool parentHierarchy)
	{
		m_ActiveHierarchy = parentHierarchy && m_ActiveSelf;

		const auto& children = m_Transform->GetChildren();
		for (auto child : children)
		{
			auto owner = child->GetGameObject();
			owner->updateActiveInHierarchy(m_ActiveHierarchy);
		}
	}
}