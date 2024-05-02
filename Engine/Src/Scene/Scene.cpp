#include "divepch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Core/CoreDefs.h"
#include "Components/Component.h"

namespace Dive
{
	static constexpr uint64_t FIRST_ID = 0x1;
	static constexpr uint64_t LAST_ID = 0xffffffffffffffff;

	Scene::Scene(int index, const std::string& name)
		: m_Index(index)
		, m_Name(name)
		, m_CurGameObjectID(FIRST_ID)
		, m_bDirty(true)
		, m_bLoaded(false)
	{
	}

	Scene::~Scene()
	{
		Clear();

		DV_CORE_TRACE("씬(index: {0:d}, name: {1:s}) 소멸", m_Index, m_Name);
	}
	
	void Scene::Clear()
	{
		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end(); ++it)
			DV_DELETE(it->second);
		m_GameObjects.clear();
		m_CurGameObjectID = FIRST_ID;

		m_bLoaded = false;
	}

	// 일단 간단하게 만들었다.
	void Scene::Update()
	{
		for (auto gameObject : m_GameObjects)
			gameObject.second->Update();
	}

	bool Scene::LoadFromFile(const std::string& fileName)
	{
		m_bLoaded = true;

		return true;
	}

	bool Scene::SaveToFile(const std::string& fileName)
	{
		return false;
	}

	GameObject* Scene::CreateGameObject(const std::string& name)
	{
		auto id = getFreeGameObjectID();
		if (id == 0)
		{
			DV_CORE_ERROR("더이상 새로운 GameObjecrt를 생성할 수 없습니다.");
			return nullptr;
		}

		auto pNewGameObject = new GameObject(this, name);
		DV_CORE_ASSERT(pNewGameObject);

		m_GameObjects[id] = pNewGameObject;
		pNewGameObject->SetID(id);

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

		if (!m_bDirty)
			m_bDirty = true;

		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end(); ++it)
		{
			if (it->first == id)
			{
				if (it->second->HasChild())
				{
					for (auto* pChild : it->second->GetChildren())
					{
						RemoveGameObjectByID(pChild->GetID());
					}

					if (!it->second->IsRoot())
						it->second->SetParent(nullptr);
				}

				it->second->MarkRemoveTarget();
			}
		}
	}

	GameObject* Scene::GetGameObjectByID(uint64_t id)
	{
		auto it = m_GameObjects.begin();
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
			if(it->second->IsRoot())
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