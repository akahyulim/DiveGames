#include "stdafx.h"
#include "World.h"
#include "WorldSerializer.h"
#include "GameObject.h"
#include "components/Transform.h"
#include "components/Camera.h"
#include "components/Light.h"
#include "components/MeshRenderer.h"
#include "rendering/Material.h"

using namespace DirectX;

namespace Dive
{
	static float ComputeSortKey(const MeshRenderer* meshRenderer, const Camera* camera)
	{
		const BoundingBox& box = meshRenderer->GetBoundingBox();
		XMFLOAT3 center = box.GetCenter();

		XMVECTOR camPos = camera->GetTransform()->GetPositionVector();
		XMVECTOR objPos = XMLoadFloat3(&center);
		XMVECTOR diff = XMVectorSubtract(objPos, camPos);
		XMVECTOR distSq = XMVector3LengthSq(diff);

		float distance;
		XMStoreFloat(&distance, distSq);
		return distance; // 정렬 키: 거리 제곱
	}

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

	// 정렬 및 절두체 컬링을 위해 카메라를 사용하므로
	// Update가 아니 개별 호출 메서드로 만들었다.
	// 허나 현재 위의 기능을 하지 못한다.
	void World::CullAndSort(Camera* camera)
	{
		assert(camera);

		m_Lights.clear();
		m_Opaques.clear();
		m_Transparents.clear();

		const auto& frustum = camera->GetFrustum();

		for (auto& [instanceID, gameObject] : m_GameObjectMap)
		{
			if (!gameObject->IsActiveSelf() || !gameObject->IsActiveHierarchy())
				continue;

			if (gameObject->HasComponent<Light>())
			{
				m_Lights.push_back(gameObject->GetComponent<Light>());
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
					m_Opaques.push_back(meshRenderer);
					break;
				case eRenderingMode::Transparent:
					m_Transparents.push_back(meshRenderer);
					break;
				default:
					break;
				}
			}
		}

		std::sort(m_Opaques.begin(), m_Opaques.end(),
			[camera](MeshRenderer* a, MeshRenderer* b)
			{
				return ComputeSortKey(a, camera) < ComputeSortKey(b, camera); // 거리 오름차순 (선택적)
			});

		std::sort(m_Transparents.begin(), m_Transparents.end(),
			[camera](MeshRenderer* a, MeshRenderer* b)
			{
				return ComputeSortKey(a, camera) > ComputeSortKey(b, camera); // 거리 내림차순
			});
	}

	GameObject* World::CreateGameObject(const std::string& name)
	{
		auto gameObject = new GameObject(this, name);
		gameObject->AddComponent<Transform>();

		m_RootGameObjects.push_back(gameObject);
		m_GameObjectMap[gameObject->GetInstanceID()] = gameObject;
	
		return gameObject;
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
				DV_DELETE(it->second);
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
		return it != m_GameObjectMap.end() ? it->second : nullptr;
	}

	bool World::Raycast(const Ray& ray, RaycastHit* outHit)
	{
		assert(outHit);
		outHit->distance = FLT_MAX;
		outHit->hitObject = nullptr;

		for (auto& [instanceID, gameObject] : m_GameObjectMap)
		{
			if (!gameObject->IsActiveSelf() || !gameObject->IsActiveHierarchy())
				continue;

			auto meshRenderer = gameObject->GetComponent<MeshRenderer>();
			if (!meshRenderer)
				continue;

			float hitDistance;
			XMFLOAT3 hitPoint;
			XMFLOAT3 hitNormal;

			if (meshRenderer->Intersects(ray, &hitDistance, &hitPoint, &hitNormal))
			{
				if (hitDistance < outHit->distance)
				{
					outHit->distance = hitDistance;
					outHit->point = hitPoint;
					outHit->normal = hitNormal;
					outHit->hitObject = gameObject;
				}
			}
		}

		return outHit->hitObject != nullptr;
	}

}
