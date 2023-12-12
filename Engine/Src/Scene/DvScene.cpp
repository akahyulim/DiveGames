#include "divepch.h"
#include "DvScene.h"
#include "GameObject.h"
#include "Components/Component.h"
#include "Components/Transform.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	static constexpr uint64_t FIRST_ID = 0x1;
	static constexpr uint64_t LAST_ID = 0xffffffffffffffff;

	DvScene::DvScene(const std::string name)
		: m_Name(name)
		, m_CurGameObjectID(FIRST_ID)
		, m_CurComponentID(FIRST_ID)
		, m_bDirty(false)
	{
	}

	DvScene::~DvScene()
	{
		Clear();
	}

	void DvScene::New()
	{
		Clear();
	}
	
	void DvScene::Clear()
	{
		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end(); ++it)
			DV_DELETE(it->second);
		m_GameObjects.clear();
		m_Components.clear();

		m_CurGameObjectID = FIRST_ID;
		m_CurComponentID = FIRST_ID;
	}
	
	void DvScene::Update()
	{
		for (auto pGameObject : m_GameObjects)
			pGameObject.second->Update();

		// 스파르탄 최신버전에선 이 방법이 아니다.
		if (m_bDirty)
		{
			auto it = m_GameObjects.begin();
			for (it; it != m_GameObjects.end();)
			{
				if (it->second->IsRemovedTarget())
				{
					DV_DELETE(it->second);
					it = m_GameObjects.erase(it);
				}
				else
					it++;
			}

			m_bDirty = false;
		}

		// 스파르탄은 resolved가 되면 WorldResolved라는 이벤트에 entity 벡터를 실어 Renderer로 보낸다.
		// 하지만 이 경우 하나의 World == View만 관리가 가능하다.
	}
	
	GameObject* DvScene::CreateGameObject(const std::string& name)
	{
		auto id = getFreeGameObjectID();
		if (id == 0)
		{
			DV_CORE_ERROR("더이상 새로운 GameObjecrt를 생성할 수 없습니다.");
			return nullptr;
		}

		auto pNewGameObject = new GameObject(name);
		DV_CORE_ASSERT(pNewGameObject);

		m_GameObjects[id] = pNewGameObject;
		pNewGameObject->SetID(id);

		return pNewGameObject;
	}
	
	void DvScene::RemoveGameObject(GameObject* pGameObject)
	{
		if (!pGameObject || m_GameObjects.empty())
			return;

		RemoveGameObjectByID(pGameObject->GetID());
	}
	
	void DvScene::RemoveGameObjectByID(uint64_t id)
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
	
	GameObject* DvScene::GetGameObjectByID(uint64_t id)
	{
		auto it = m_GameObjects.begin();
		return it != m_GameObjects.end() ? it->second : nullptr;
	}
	
	bool DvScene::ExistsGameObject(GameObject* pGameObject)
	{
		if (!pGameObject)
			return false;

		return ExistsGameObjectByID(pGameObject->GetID());
	}
	
	bool DvScene::ExistsGameObjectByID(uint64_t id)
	{
		auto it = m_GameObjects.find(id);
		return it != m_GameObjects.end();
	}
	
	std::vector<GameObject*> DvScene::GetRootGameObjects()
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
	
	std::vector<GameObject*> DvScene::GetAllGameObjects()
	{
		std::vector<GameObject*> allGameObjects;

		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end(); ++it)
			allGameObjects.emplace_back(it->second);

		return allGameObjects;
	}
	
	uint64_t DvScene::GetGameObjectsCount()
	{
		return static_cast<uint64_t>(m_GameObjects.size());
	}
	
	void DvScene::RegisterComponent(Component* pComponent, uint64_t id)
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
	
	void DvScene::DeregisterComponent(Component* pComponent)
	{
		if (!pComponent)
			return;

		DeregisterComponentByID(pComponent->GetID());
	}
	
	void DvScene::DeregisterComponentByID(uint64_t id)
	{
		if (!GetComponent(id))
			return;

		if (m_Components.find(id) != m_Components.end())
			m_Components.erase(id);
	}
	
	Component* DvScene::GetComponent(uint64_t id)
	{
		auto it = m_Components.find(id);
		return it != m_Components.end() ? it->second : nullptr;
	}

	uint64_t DvScene::getFreeGameObjectID()
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
	
	uint64_t DvScene::getFreeComponentID()
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