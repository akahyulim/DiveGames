#include "stdafx.h"
#include "World.h"
#include "WorldSerializer.h"
#include "GameObject.h"
#include "components/Transform.h"
#include "components/Camera.h"
#include "components/Light.h"
#include "components/MeshRenderer.h"
#include "rendering/Material.h"

namespace Dive
{
	World::World(const std::string& name)
		: m_name(name)
	{
		DV_LOG(World, info, "생성: {}", m_name);
	}

	World::~World()
	{
		Clear();
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

	// 정렬 및 절두체 컬링을 위해 카메라를 사용하므로
	// Update가 아니 개별 호출 메서드로 만들었다.
	// 허나 현재 위의 기능을 하지 못한다.
	void World::CullAndSort(Camera* camera)
	{
		assert(camera);

		m_lights.clear();
		m_opaqueMeshRenderers.clear();
		m_transparentMeshRenderers.clear();

		const auto& frustum = camera->GetFrustum();

		for (auto& [instanceID, gameObject] : m_gameObjectMap)
		{
			if (!gameObject->IsActiveSelf() || !gameObject->IsActiveHierarchy())
				continue;

			if (gameObject->HasComponent<Light>())
			{
				m_lights.push_back(gameObject->GetComponent<Light>());
			}
			else if (gameObject->HasComponent<MeshRenderer>())
			{
				auto meshRenderer = gameObject->GetComponent<MeshRenderer>();
				auto material = meshRenderer->GetMaterial();
				// boundingBox를 통해 Frustum Culling 확인

				// 실제로는 정렬까지...?
				switch (material->GetRenderingMode())
				{
				case eRenderingMode::Opqaue:
					m_opaqueMeshRenderers.push_back(meshRenderer);
					break;
				case eRenderingMode::Transparent:
					m_transparentMeshRenderers.push_back(meshRenderer);
					break;
				default:
					break;
				}
			}
		}
	}

	GameObject* World::CreateGameObject(const std::string& name)
	{
		auto gameObject = new GameObject(this, name);
		gameObject->AddComponent<Transform>();

		m_rootGameObjects.push_back(gameObject);
		m_gameObjectMap[gameObject->GetInstanceID()] = gameObject;
	
		return gameObject;
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
			allGameObjects.push_back(gameObject);

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
			{
				DV_DELETE(it->second);
				m_gameObjectMap.erase(it);
			}
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
		return it != m_gameObjectMap.end() ? it->second : nullptr;
	}
}
