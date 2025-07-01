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
		m_DestroyQueue.clear();
		m_RootGameObjects.clear();
		m_GameObjectMap.clear();
	}

	void World::Update()
	{
		for (auto& gameObject : m_RootGameObjects)
			gameObject->Update();

		FlushDestoryQueue();
	}

	GameObject* World::CreateGameObject(const std::string& name)
	{
		auto gameObject = std::make_unique<GameObject>(name);
		gameObject->AddComponent<Transform>();

		GameObject* ptr = gameObject.get();
		m_RootGameObjects.push_back(ptr);

		m_GameObjectMap[gameObject->GetInstanceID()] = std::move(gameObject);
	
		return ptr;
	}

	void World::AttachRoot(GameObject* gameObject)
	{
		assert(gameObject);

		auto it = std::find(m_RootGameObjects.begin(), m_RootGameObjects.end(), gameObject);
		if (it != m_RootGameObjects.end())
			return;

		m_RootGameObjects.push_back(gameObject);
	}

	void World::AttachRoot(UINT64 instanceID)
	{
		if (!HasGameObject(instanceID))
			return;

		auto gameObject = FindGameObject(instanceID);
		auto it = std::find(m_RootGameObjects.begin(), m_RootGameObjects.end(), gameObject);
		if (it != m_RootGameObjects.end())
			return;

		m_RootGameObjects.push_back(gameObject);
	}

	void World::DetachRoot(GameObject* gameObject)
	{
		assert(gameObject);

		auto it = std::find(m_RootGameObjects.begin(), m_RootGameObjects.end(), gameObject);
		if (it != m_RootGameObjects.end())
			m_RootGameObjects.erase(it);
	}

	void World::DetachRoot(UINT64 instanceID)
	{
		if (!HasGameObject(instanceID))
			return;

		auto gameObject = FindGameObject(instanceID);
		auto it = std::find(m_RootGameObjects.begin(), m_RootGameObjects.end(), gameObject);
		if (it != m_RootGameObjects.end())
			m_RootGameObjects.erase(it);
	}

	std::vector<GameObject*> World::GetAllGameObjects()
	{
		std::vector<GameObject*> allGameObjects;
		allGameObjects.reserve(AllGameObjectCount());

		for (auto& [instanceID, gameObject] : m_GameObjectMap)
			allGameObjects.push_back(gameObject.get());

		return allGameObjects;
	}


	void World::DestroyGameObject(GameObject* gameObject)
	{
		assert(gameObject);
		assert(HasGameObject(gameObject->GetInstanceID()));

		gameObject->Destory();
	}

	void World::DestroyGameObject(UINT64 instanceID)
	{
		if (!HasGameObject(instanceID))
			return;

		auto gameObject = FindGameObject(instanceID);
		if (gameObject) return;

		gameObject->Destory();
	}

	void World::QueueDestroy(GameObject* gameObject)
	{
		assert(gameObject);
		assert(HasGameObject(gameObject->GetInstanceID()));

		if (m_DestroyQueue.find(gameObject->GetInstanceID()) == m_DestroyQueue.end())
			m_DestroyQueue.insert(gameObject->GetInstanceID());
	}

	void World::QueueDestroy(UINT64 instanceID)
	{
		if (!HasGameObject(instanceID))
			return;

		if (m_DestroyQueue.find(instanceID) == m_DestroyQueue.end())
			m_DestroyQueue.insert(instanceID);
	}

	void World::FlushDestoryQueue()
	{
		for (auto instanceID : m_DestroyQueue)
		{
			auto it = m_GameObjectMap.find(instanceID);
			if (it != m_GameObjectMap.end())
			{
				DV_LOG(World, trace, "Destroying GameObject: {}, {}", it->second->GetName(), instanceID);
				m_GameObjectMap.erase(it);
			}
		}

		m_DestroyQueue.clear();
	}

	bool World::HasGameObject(UINT64 instanceID)
	{
		return m_GameObjectMap.find(instanceID) != m_GameObjectMap.end();
	}

	GameObject* World::FindGameObject(UINT64 instanceID)
	{
		auto it = m_GameObjectMap.find(instanceID);
		return it != m_GameObjectMap.end() ? it->second.get() : nullptr;
	}

	std::unique_ptr<World> WorldManager::s_ActiveWorld = nullptr;

	// 유니티의 경우 빈 이름, 이미 존재하는 Scene의 이름은 안된다.
	World* WorldManager::CreateWorld(const std::string& name)
	{
		if (s_ActiveWorld)	s_ActiveWorld.reset();
		
		s_ActiveWorld = std::make_unique<World>(name);
		return s_ActiveWorld.get();
	}

	World* WorldManager::GetActiveWorld()
	{
		return s_ActiveWorld.get();
	}
}