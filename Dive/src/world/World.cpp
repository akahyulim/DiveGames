#include "stdafx.h"
#include "World.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Common.h"

namespace Dive
{
	World::World(const std::string& name)
		: m_Name(name)
	{
		DV_LOG(World, info, "생성: {}", m_Name);
	}

	World::~World()
	{
		Clear();
		DV_LOG(World, info, "소멸: {}", m_Name);
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

	void World::CullAndSort(Camera* camera)
	{
		assert(camera);

		m_OpaqueMeshRenderers.clear();
		m_TransparentMeshRenderers.clear();

		const auto& frustum = camera->GetFrustum();

		for (auto& [instanceID, gameObject] : m_GameObjectMap)
		{
			if (gameObject->HasComponent<MeshRenderer>())
			{
				auto meshRenderer = gameObject->GetComponent<MeshRenderer>();
				// boundingBox를 통해 Frustum Culling 확인

				// 실제로는 정렬까지...?
				if (meshRenderer->IsTransparent())
					m_TransparentMeshRenderers.push_back(meshRenderer);
				else
					m_OpaqueMeshRenderers.push_back(meshRenderer);
			}
		}

		// 코파일럿이 보여준 예
		/*
		ClearAllRenderQueues();

		Frustum frustum = camera.ComputeFrustum();

	    for (auto& go : m_GameObjects) {
        if (go->meshRenderer && go->meshRenderer->IsVisible(frustum)) {
            auto& renderer = *go->meshRenderer;
            if (renderer.IsTransparent())
                m_TransparentStatic.push_back(&renderer);
            else
                m_OpaqueStatic.push_back(&renderer);
        }

        if (go->skinnedRenderer && go->skinnedRenderer->IsVisible(frustum)) {
            auto& renderer = *go->skinnedRenderer;
            if (renderer.IsTransparent())
                m_TransparentSkinned.push_back(&renderer);
            else
                m_OpaqueSkinned.push_back(&renderer);
        }
    }

		*/
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

	void World::AttachRoot(uint64_t instanceID)
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

	void World::DetachRoot(uint64_t instanceID)
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

		if (m_DestroyQueue.find(gameObject->GetInstanceID()) == m_DestroyQueue.end())
			m_DestroyQueue.insert(gameObject->GetInstanceID());
	}

	void World::QueueDestroy(uint64_t instanceID)
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
				DV_LOG(World, info, "게임 오브젝트 제거: {}, {}", it->second->GetName(), instanceID);
				m_GameObjectMap.erase(it);
			}
		}

		m_DestroyQueue.clear();
	}

	bool World::HasGameObject(uint64_t instanceID)
	{
		return m_GameObjectMap.find(instanceID) != m_GameObjectMap.end();
	}

	GameObject* World::FindGameObject(uint64_t instanceID)
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

	void WorldManager::Clear()
	{
		if (s_ActiveWorld)
			s_ActiveWorld.reset();

		DV_LOG(WorldManager, info, "클리어 완료");
	}

	World* WorldManager::GetActiveWorld()
	{
		return s_ActiveWorld.get();
	}
}
