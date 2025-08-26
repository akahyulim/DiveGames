#include "stdafx.h"
#include "World.h"
#include "GameObject.h"
#include "components/Transform.h"
#include "components/Camera.h"
#include "components/Light.h"
#include "components/MeshRenderer.h"
#include "resource/ResourceManager.h"

namespace Dive
{
	World::World(const std::string& name)
		: m_name(name)
	{
		ResourceManager::Initialize();

		DV_LOG(World, info, "생성: {}", m_name);
	}

	World::~World()
	{
		Clear();
		ResourceManager::Clear();
		DV_LOG(World, info, "소멸: {}", m_name);
	}

	void World::Clear()
	{
		m_destroyQueue.clear();
		m_rootGameObjects.clear();
		m_gameObjectMap.clear();
	}

	void World::Update()
	{
		for (auto& gameObject : m_rootGameObjects)
			gameObject->Update();

		FlushDestoryQueue();
	}

	void World::CullAndSort(Camera* camera)
	{
		assert(camera);

		m_lights.clear();
		m_opaqueMeshRenderers.clear();
		m_transparentMeshRenderers.clear();

		const auto& frustum = camera->GetFrustum();

		for (auto& [instanceID, gameObject] : m_gameObjectMap)
		{
			if (!gameObject->IsActiveSelf())
				continue;

			if (gameObject->HasComponent<Light>())
			{
				m_lights.push_back(gameObject->GetComponent<Light>());
			}
			else if (gameObject->HasComponent<MeshRenderer>())
			{
				auto meshRenderer = gameObject->GetComponent<MeshRenderer>();
				// boundingBox를 통해 Frustum Culling 확인

				// 실제로는 정렬까지...?
				if (meshRenderer->IsTransparent())
					m_transparentMeshRenderers.push_back(meshRenderer);
				else
					m_opaqueMeshRenderers.push_back(meshRenderer);
			}
		}
	}

	GameObject* World::CreateGameObject(const std::string& name)
	{
		auto gameObject = std::make_unique<GameObject>(name);
		gameObject->AddComponent<Transform>();

		GameObject* ptr = gameObject.get();
		m_rootGameObjects.push_back(ptr);

		m_gameObjectMap[gameObject->GetInstanceID()] = std::move(gameObject);
	
		return ptr;
	}

	void World::AttachRoot(GameObject* gameObject)
	{
		assert(gameObject);

		auto it = std::find(m_rootGameObjects.begin(), m_rootGameObjects.end(), gameObject);
		if (it != m_rootGameObjects.end())
			return;

		m_rootGameObjects.push_back(gameObject);
	}

	void World::AttachRoot(uint64_t instanceID)
	{
		if (!HasGameObject(instanceID))
			return;

		auto gameObject = FindGameObject(instanceID);
		auto it = std::find(m_rootGameObjects.begin(), m_rootGameObjects.end(), gameObject);
		if (it != m_rootGameObjects.end())
			return;

		m_rootGameObjects.push_back(gameObject);
	}

	void World::DetachRoot(GameObject* gameObject)
	{
		assert(gameObject);

		auto it = std::find(m_rootGameObjects.begin(), m_rootGameObjects.end(), gameObject);
		if (it != m_rootGameObjects.end())
			m_rootGameObjects.erase(it);
	}

	void World::DetachRoot(uint64_t instanceID)
	{
		if (!HasGameObject(instanceID))
			return;

		auto gameObject = FindGameObject(instanceID);
		auto it = std::find(m_rootGameObjects.begin(), m_rootGameObjects.end(), gameObject);
		if (it != m_rootGameObjects.end())
			m_rootGameObjects.erase(it);
	}

	std::vector<GameObject*> World::GetAllGameObjects()
	{
		std::vector<GameObject*> allGameObjects;
		allGameObjects.reserve(AllGameObjectCount());

		for (auto& [instanceID, gameObject] : m_gameObjectMap)
			allGameObjects.push_back(gameObject.get());

		return allGameObjects;
	}

	void World::DestroyGameObject(GameObject* gameObject)
	{
		assert(gameObject);
		assert(HasGameObject(gameObject->GetInstanceID()));

		gameObject->Destory();
	}

	void World::DestroyGameObject(uint64_t instanceID)
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

		if (m_destroyQueue.find(gameObject->GetInstanceID()) == m_destroyQueue.end())
			m_destroyQueue.insert(gameObject->GetInstanceID());
	}

	void World::QueueDestroy(uint64_t instanceID)
	{
		if (!HasGameObject(instanceID))
			return;

		if (m_destroyQueue.find(instanceID) == m_destroyQueue.end())
			m_destroyQueue.insert(instanceID);
	}

	void World::FlushDestoryQueue()
	{
		for (auto instanceID : m_destroyQueue)
		{
			auto it = m_gameObjectMap.find(instanceID);
			if (it != m_gameObjectMap.end())
				m_gameObjectMap.erase(it);
		}
		m_destroyQueue.clear();
	}

	bool World::HasGameObject(uint64_t instanceID)
	{
		return m_gameObjectMap.find(instanceID) != m_gameObjectMap.end();
	}

	GameObject* World::FindGameObject(uint64_t instanceID)
	{
		auto it = m_gameObjectMap.find(instanceID);
		return it != m_gameObjectMap.end() ? it->second.get() : nullptr;
	}

	std::unique_ptr<World> WorldManager::s_activeWorld = nullptr;

	bool WorldManager::Initialize()
	{
		DV_SUBSCRIBE_EVENT(eEventType::Update, DV_EVENT_HANDLER_DATA_STATIC(OnUpdate));

		return true;
	}

	// 유니티의 경우 빈 이름, 이미 존재하는 Scene의 이름은 안된다.
	World* WorldManager::CreateWorld(const std::string& name)
	{
		if (s_activeWorld)	s_activeWorld.reset();
		
		s_activeWorld = std::make_unique<World>(name);
		return s_activeWorld.get();
	}

	void WorldManager::Clear()
	{
		if (s_activeWorld)
			s_activeWorld.reset();

		DV_LOG(WorldManager, info, "클리어 완료");
	}

	void WorldManager::OnUpdate(EventData data)
	{
		if (!s_activeWorld)
			return;

		s_activeWorld->Update();
	}

	World* WorldManager::GetActiveWorld()
	{
		return s_activeWorld.get();
	}
}
