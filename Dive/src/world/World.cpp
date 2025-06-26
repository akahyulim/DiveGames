#include "stdafx.h"
#include "World.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "core/CoreDefs.h"
#include "core/InstanceID.h"

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
		m_RootGameObjects.clear();
	}

	void World::Update()
	{
		for (auto& gameObject : m_RootGameObjects)
			gameObject->Update();

		FlushDestoryQueue();
	}

	std::shared_ptr<GameObject> World::CreateGameObject(const std::string& name)
	{
		return CreateGameObject(InstanceID(), name);
	}

	std::shared_ptr<GameObject> World::CreateGameObject(UINT64 instanceID, const std::string& name)
	{
		if (HasGameObject(instanceID))
			return nullptr;

		auto newGO = std::make_shared<GameObject>(instanceID, name);
		m_GameObjectMap[instanceID] = newGO;
		m_RootGameObjects.push_back(newGO);

		return newGO;
	}

	void World::AttachRoot(GameObject* gameObject)
	{
		if (!gameObject) return;

		if (!gameObject->GetTransform()->HasParent())
			m_RootGameObjects.push_back(gameObject->GetSharedPtr());
	}

	void World::DetachRoot(GameObject* gameObject)
	{
		if (!gameObject) return;

		auto it = std::find(m_RootGameObjects.begin(), m_RootGameObjects.end(), gameObject->GetSharedPtr());
		if (it != m_RootGameObjects.end())
			m_RootGameObjects.erase(it);
	}

	std::vector<std::shared_ptr<GameObject>> World::GetAllGameObjects()
	{
		std::vector<std::shared_ptr<GameObject>> allGameObjects;
		allGameObjects.reserve(AllGameObjectCount());

		for (auto& [instanceID, gameObject] : m_GameObjectMap)
			allGameObjects.push_back(gameObject);

		return allGameObjects;
	}

	void World::TraverseHierarchy(const std::function<void(GameObject*)>& visit)
	{
		for (auto& gameObject : m_RootGameObjects)
			traverseRecursive(gameObject.get(), visit);
	}

	void World::DestroyGameObject(std::shared_ptr<GameObject> gameObject)
	{
		if (gameObject)
			DestroyGameObject(gameObject->GetInstanceID());
	}

	void World::DestroyGameObject(UINT64 instanceID)
	{
		if (!HasGameObject(instanceID))
			return;

		auto gameObject = FindGameObject(instanceID);
		if (gameObject && gameObject->IsDestroyed())
			return;

		if(gameObject)
			gameObject->Destory();
	}

	void World::QueueDestory(GameObject* gameObject)
	{
		m_DestroyQueue.insert(gameObject->GetInstanceID());
	}

	void World::FlushDestoryQueue()
	{
		for (auto id : m_DestroyQueue)
		{
			auto it = m_GameObjectMap.find(id);
			if (it != m_GameObjectMap.end())
				m_GameObjectMap.erase(it);
		}

		m_DestroyQueue.clear();
	}

	bool World::HasGameObject(UINT64 instanceID)
	{
		return m_GameObjectMap.find(instanceID) != m_GameObjectMap.end();
	}

	std::shared_ptr<GameObject> World::FindGameObject(UINT64 instanceID)
	{
		auto it = m_GameObjectMap.find(instanceID);
		return it != m_GameObjectMap.end() ? it->second : nullptr;
	}

	void World::traverseRecursive(GameObject* gameObject, const std::function<void(GameObject*)>& visit)
	{
		if (!gameObject) return;

		visit(gameObject);

		for (auto childTransform : gameObject->GetTransform()->GetChildren())
		{
			if (auto childGO = childTransform->GetGameObject())
				traverseRecursive(childGO, visit);
		}
	}

	World* WorldManager::s_ActiveWorld = nullptr;

	// 유니티의 경우 빈 이름, 이미 존재하는 Scene의 이름은 안된다.
	World* WorldManager::CreateWorld(const std::string& name)
	{
		DV_DELETE(s_ActiveWorld);

		auto newWorld = new World(name);
		s_ActiveWorld = newWorld;
		return newWorld;
	}

	World* WorldManager::GetActiveWorld()
	{
		return s_ActiveWorld;
	}
}