#include "stdafx.h"
#include "GameObject.h"
#include "Core/CoreDefs.h"
#include "core/EventDispatcher.h"
#include "Components/Component.h"
#include "Components/Transform.h"
#include "Components/Camera.h"

namespace Dive
{
	GameObject::GameObject(World* world, const std::string& name)
		: m_Name(name)
		, m_World(world)
		, m_Transform(AddComponent<Transform>())
	{
	}

	GameObject::~GameObject()
	{
		for (auto& it : m_Components)
			DV_DELETE(it.second);

		DV_LOG(GameObject, debug, "deleted gameObject: {}", GetName());
	}

	void GameObject::Update()
	{
		if (!m_IsActive)
			return;

		for (auto& it : m_Components)
			it.second->Update();
	}

	void GameObject::SetName(const std::string& name)
	{
		if (m_Name != name)
		{
			m_Name = name;
			DV_FIRE_EVENT(eEventType::WorldModified);
		}
	}

	bool GameObject::AddComponent(Component* component)
	{
		DV_ASSERT(GameObject, component != nullptr);

		size_t typeHash = component->GetTypeHash();
		if(m_Components.find(typeHash)!= m_Components.end())
			return false;

		m_Components[typeHash] = component;
		component->SetGameObject(this);

		return true;
	}

	bool GameObject::HashComponent(size_t typeHash) const
	{
		auto it = m_Components.find(typeHash);
		return it != m_Components.end();
	}

	Component* GameObject::GetComponent(size_t typeHash) const
	{
		auto it = m_Components.find(typeHash);
		if (it != m_Components.end())
			return it->second;

		return nullptr;
	}

	// Camera와 MeshRenderer처럼 유효하지 않은 조합을 걸러내야 한다.
	bool GameObject::ComponentValidator(GameObject* gameObject, Component* component)
	{
		return false;
	}
}