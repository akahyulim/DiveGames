#include "stdafx.h"
#include "GameObject.h"
#include "Core/CoreDefs.h"
#include "Components/Component.h"
#include "Components/Transform.h"

namespace Dive
{
	GameObject::GameObject(World* pWorld, const std::string& name)
		: m_Name(name)
		, m_ID(0)
		, m_pWorld(pWorld)
		, m_bActive(true)
		, m_bMarkedTarget(false)
	{
		AddComponent<Transform>();
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

	void GameObject::SaveToYaml(YAML::Node& config)
	{
		config["Name"] = m_Name;
		config["ID"] = m_ID;

		YAML::Node components;
		for (auto& [typeHasn, pComponent] : m_Components)
		{
			YAML::Node component;

		}
	}

	void GameObject::LoadFromYaml(const YAML::Node& config)
	{
		m_Name = config["Name"].as<std::string>();
		m_ID = config["ID"].as<uint64_t>();
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