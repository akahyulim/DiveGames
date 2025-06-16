#include "stdafx.h"
#include "World.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "core/CoreDefs.h"

namespace Dive
{
	World::World(const std::string& name)
		: m_Name(name)
	{
	}

	World::~World()
	{
		Clear();
	}

	void World::Clear()
	{
		for (auto& [id, gameObject] : m_GameObjects)
		{
			DV_DELETE(gameObject);
		}
		m_GameObjects.clear();
	}

	void World::Update()
	{
		for (auto& [instanceID, gameObject] : m_GameObjects)
		{
			gameObject->Update();
		}
	}

	GameObject* World::CreateGameObject(const std::string& name)
	{
		auto gameObject = new GameObject(name);
		m_GameObjects[gameObject->GetInstanceID()] = gameObject;
		return gameObject;
	}

	GameObject* World::CreateGameObjectWithInstanceID(UINT64 instanceID, const std::string& name)
	{
		if (m_GameObjects.find(instanceID) != m_GameObjects.end())
		{
			DV_LOG(World, err, "GameObject with InstanceID {} already exists.", instanceID);
			return nullptr;
		}

		auto gameObject = new GameObject(instanceID, name);
		m_GameObjects[instanceID] = gameObject;
		return gameObject;
	}

	void World::DeleteGameObject(GameObject* gameObject)
	{
		if (!gameObject)
			return;

		DeleteGameObjectByInstanceID(gameObject->GetInstanceID());
	}
	
	void World::DeleteGameObjectByInstanceID(UINT64 instanceID)
	{
		auto it = m_GameObjects.find(instanceID);
		if (it == m_GameObjects.end())
		{
			DV_LOG(World, warn, "GameObject with InstanceID {} does not exist.", instanceID);
		}
		
		auto transform = it->second->GetTransform();
		if (transform->HasChildren())
		{
			auto children = transform->GetChildren();
			for (auto child : children)
			{
				DeleteGameObjectByInstanceID(child->GetGameObject()->GetInstanceID());
			}
		}
		
		if (transform->HasParent())
		{
			transform->GetParent()->RemoveChild(transform);
		}

		DV_DELETE(it->second);
		m_GameObjects.erase(it);	
	}

	GameObject* World::GetGameObjectByInstanceID(UINT64 instanceID)
	{
		auto it = m_GameObjects.find(instanceID);
		if (it != m_GameObjects.end())
		{
			return it->second;
		}
		return nullptr;
	}

	bool World::ExistsGameObject(GameObject* gameObject)
	{
		if (!gameObject)
			return false;;

		return ExistsGameObjectByInstanceID(gameObject->GetInstanceID());
	}
	
	bool World::ExistsGameObjectByInstanceID(UINT64 instanceID)
	{
		return m_GameObjects.find(instanceID) != m_GameObjects.end();
	}
	
	std::vector<GameObject*> World::GetRootGameObjects()
	{
		std::vector<GameObject*> roots;
		for (auto& [instanceID, gameObject] : m_GameObjects)
		{
			if (!gameObject->GetTransform()->HasParent())
				roots.push_back(gameObject);
		}

		return roots;
	}
	
	std::vector<GameObject*> World::GetAllGameObjects()
	{
		std::vector<GameObject*> gameObjects;
		gameObjects.reserve(m_GameObjects.size());

		for (const auto& [id, gameObject] : m_GameObjects)
		{
			gameObjects.push_back(gameObject);
		}

		return gameObjects;
	}
	
	UINT64 World::GameObjectsCount()
	{
		return static_cast<UINT64>(m_GameObjects.size());
	}
}