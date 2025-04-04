#include "stdafx.h"
#include "GameObject.h"
#include "Core/CoreDefs.h"
#include "core/EventDispatcher.h"
#include "Components/Component.h"
#include "Components/Transform.h"

namespace Dive
{
	GameObject::GameObject(Scene* pScene, const std::string& name)
		: m_Name(name)
		, m_ID(0)
		, m_Tag("Untagged")
		, m_pScene(pScene)
		, m_bActive(true)
		, m_bMarkedTarget(false)
		, m_pTransform(AddComponent<Transform>())
	{
	}

	GameObject::~GameObject()
	{
		for (auto& it : m_Components)
			DV_DELETE(it.second);

		DV_LOG(GameObject, trace, "게임오브젝트({0:s}, {1:d}) 소멸", GetName(), GetID());
	}

	void GameObject::Update()
	{
		if (!m_bActive)
			return;

		for (auto& it : m_Components)
			it.second->Update();
	}

	void GameObject::SetName(const std::string& name)
	{
		if (m_Name != name)
		{
			m_Name = name;
			DV_FIRE_EVENT(eEventType::SceneModified);
		}
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
}
