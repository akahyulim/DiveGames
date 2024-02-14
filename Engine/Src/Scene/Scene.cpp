#include "divepch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Core/CoreDefs.h"
#include "Components/Component.h"
#include "Components/Transform.h"

namespace Dive
{
	static constexpr uint64_t FIRST_ID = 0x1;
	static constexpr uint64_t LAST_ID = 0xffffffffffffffff;

	Scene::Scene(const std::string& name)
		: m_Name(name)
		, m_CurGameObjectID(FIRST_ID)
		, m_CurComponentID(FIRST_ID)
		, m_bDirty(true)
	{
	}

	Scene::~Scene()
	{
	}
	
	void Scene::Clear()
	{
		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end(); ++it)
			DV_DELETE(it->second);
		m_GameObjects.clear();
		m_Components.clear();

		m_CurGameObjectID = FIRST_ID;
		m_CurComponentID = FIRST_ID;
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
				auto* pTransform = it->second->GetComponent<Transform>();
				if (pTransform)
				{
					if (pTransform->HasChild())
					{
						for (auto* pChild : pTransform->GetChildren())
							RemoveGameObjectByID(pChild->GetGameObject()->GetID());
					}

					if (pTransform->HasParent())
						pTransform->SetParent(nullptr);
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
			auto pTransform = it->second->GetComponent<Transform>();
			if (!pTransform)
				continue;

			if (pTransform->HasParent())
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

	void Scene::RegisterComponent(Component* pComponent, uint64_t id)
	{
		if (!pComponent)
			return;

		if (id == 0 || GetComponent(id))
		{
			id = getFreeComponentID();
			if (id == 0)
			{
				DV_CORE_ERROR("더이상 Component를 등록할 수 없습니다.");
				return;
			}
		}

		m_Components[id] = pComponent;
		pComponent->SetID(id);
	}

	void Scene::DeregisterComponent(Component* pComponent)
	{
		if (!pComponent)
			return;

		DeregisterComponentByID(pComponent->GetID());
	}

	void Scene::DeregisterComponentByID(uint64_t id)
	{
		if (!GetComponent(id))
			return;

		if (m_Components.find(id) != m_Components.end())
			m_Components.erase(id);
	}

	Component* Scene::GetComponent(uint64_t id)
	{
		auto it = m_Components.find(id);
		return it != m_Components.end() ? it->second : nullptr;
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

	uint64_t Scene::getFreeComponentID()
	{
		auto checkID = m_CurComponentID;

		for (;;)
		{
			auto freeID = m_CurComponentID;

			if (m_CurComponentID < LAST_ID)
				++m_CurComponentID;
			else
				m_CurComponentID = FIRST_ID;

			if (checkID == m_CurComponentID)
				return 0;
			else if (!m_Components[freeID])
				return freeID;
		}
	}
}