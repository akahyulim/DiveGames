#include "stdafx.h"
#include "Scene.h"
#include "SceneSerializer.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "core/CoreDefs.h"
#include "core/EventDispatcher.h"

namespace Dive
{
	static constexpr uint64_t FIRST_ID = 0x1;
	static constexpr uint64_t LAST_ID = 0xffffffffffffffff;

	Scene::Scene(const std::string& name)
		: m_Name(name)
		, m_CurGameObjectID(FIRST_ID)
		, m_bDirty(true)
	{
		DV_SUBSCRIBE_EVENT(Dive::eEventType::SceneModified, DV_EVENT_HANDLER(OnModified));
	}

	Scene::~Scene()
	{
		Clear();
	}

	void Scene::Clear()
	{
		for (auto& [id, pGameObject] : m_GameObjects)
		{
			DV_DELETE(pGameObject);
		}
		m_GameObjects.clear();
		m_CurGameObjectID = FIRST_ID;
		m_bDirty = true;
	}

	void Scene::Tick()
	{
		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end();)
		{
			if (it->second->IsRemovedTarget())
			{
				DV_DELETE(it->second);
				it = m_GameObjects.erase(it);
				m_bDirty = true;
			}
			else 
			{
				++it;
			}
		}

		for (auto& [id, pGameObject] : m_GameObjects)
		{
			pGameObject->Update();
		}
	}

	GameObject* Scene::CreateGameObject(const std::string& name)
	{
		auto id = getFreeGameObjectID();
		if (id == 0)
		{
			DV_LOG(Scene, err, "더이상 새로운 GameObject를 생성할 수 없습니다.");
			return nullptr;
		}

		auto pNewGameObject = new GameObject(this, name);
		m_GameObjects[id] = pNewGameObject;
		pNewGameObject->SetID(id);

		m_bDirty = true;

		return pNewGameObject;
	}

	void Scene::RemoveGameObject(GameObject* pGameObject)
	{
		if (!pGameObject || m_GameObjects.empty())
			return;

		RemoveGameObjectByID(pGameObject->GetID());
	}

	void Scene::RemoveGameObjectByID(uint64_t id)
	{
		if (!id || m_GameObjects.empty())
			return;

		auto it = m_GameObjects.find(id);
		if (it != m_GameObjects.end())
		{
			auto pTransform = it->second->GetComponent<Transform>();
			if (pTransform->HasChild())
			{
				for (auto pChild : pTransform->GetChildren())
				{
					RemoveGameObjectByID(pChild->GetGameObject()->GetID());
				}
			}

			if (!pTransform->IsRoot())
			{
				pTransform->SetParent(nullptr);
			}

			it->second->MarkRemoveTarget();
		}
	}

	GameObject* Scene::GetGameObjectByID(uint64_t id)
	{
		auto it = m_GameObjects.find(id);
		return it != m_GameObjects.end() ? it->second : nullptr;
	}

	bool Scene::ExistsGameObject(GameObject* pGameObject)
	{
		if (!pGameObject)
			return false;

		return ExistsGameObjectByID(pGameObject->GetID());
	}

	bool Scene::ExistsGameObjectByID(uint64_t id)
	{
		auto it = m_GameObjects.find(id);
		return it != m_GameObjects.end();
	}

	std::vector<GameObject*> Scene::GetRootGameObjects()
	{
		std::vector<GameObject*> rootGameObjects;

		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end(); ++it)
		{
			if (it->second->GetComponent<Transform>()->IsRoot())
				rootGameObjects.emplace_back(it->second);
		}

		return rootGameObjects;
	}

	std::vector<GameObject*> Scene::GetAllGameObjects()
	{
		std::vector<GameObject*> allGameObjects;

		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end(); ++it)
			allGameObjects.emplace_back(it->second);

		return allGameObjects;
	}

	uint64_t Scene::GetGameObjectsCount()
	{
		return static_cast<uint64_t>(m_GameObjects.size());
	}

	void Scene::SetName(const std::string& name)
	{
		if (m_Name != name)
		{
			m_Name = name;
			m_bDirty = true;
		}
	}

	void Scene::OnModified()
	{
		m_bDirty = true;
	}

	uint64_t Scene::getFreeGameObjectID()
	{
		auto checkID = m_CurGameObjectID;

		for (;;)
		{
			auto freeID = m_CurGameObjectID;

			if (m_CurGameObjectID < LAST_ID)
				++m_CurGameObjectID;
			else
				m_CurGameObjectID = FIRST_ID;

			if (checkID == m_CurGameObjectID)
				return 0;
			else if (!m_GameObjects[freeID])
				return freeID;
		}
	}
}
