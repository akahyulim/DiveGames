#include "divepch.h"
#include "DvScene.h"
#include "DvGameObject.h"
#include "Core/CoreDefs.h"
#include "Components/DvComponent.h"
#include "Components/DvTransform.h"

namespace Dive
{
	static constexpr uint64_t FIRST_ID = 0x1;
	static constexpr uint64_t LAST_ID = 0xffffffffffffffff;

	DvScene::DvScene(const std::string& name)
		: m_Name(name)
		, m_CurGameObjectID(FIRST_ID)
		, m_CurComponentID(FIRST_ID)
		, m_bDirty(true)
	{
	}

	DvScene::~DvScene()
	{
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

	DvGameObject* DvScene::CreateGameObject(const std::string& name)
	{
		auto id = getFreeGameObjectID();
		if (id == 0)
		{
			DV_CORE_ERROR("더이상 새로운 GameObjecrt를 생성할 수 없습니다.");
			return nullptr;
		}

		auto pNewGameObject = new DvGameObject(this, name);
		DV_CORE_ASSERT(pNewGameObject);

		m_GameObjects[id] = pNewGameObject;
		pNewGameObject->SetID(id);

		return pNewGameObject;
	}

	void DvScene::RemoveGameObject(DvGameObject* pGameObject)
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
				auto* pTransform = it->second->GetComponent<DvTransform>();
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

	DvGameObject* DvScene::GetGameObjectByID(uint64_t id)
	{
		auto it = m_GameObjects.begin();
		return it != m_GameObjects.end() ? it->second : nullptr;
	}

	bool DvScene::ExistsGameObject(DvGameObject* pGameObject)
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

	std::vector<DvGameObject*> DvScene::GetRootGameObjects()
	{
		std::vector<DvGameObject*> rootGameObjects;

		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end(); ++it)
		{
			auto pTransform = it->second->GetComponent<DvTransform>();
			if (!pTransform)
				continue;

			if (pTransform->HasParent())
				rootGameObjects.emplace_back(it->second);
		}

		return rootGameObjects;
	}

	std::vector<DvGameObject*> DvScene::GetAllGameObjects()
	{
		std::vector<DvGameObject*> allGameObjects;

		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end(); ++it)
			allGameObjects.emplace_back(it->second);

		return allGameObjects;
	}

	uint64_t DvScene::GetGameObjectsCount()
	{
		return static_cast<uint64_t>(m_GameObjects.size());
	}

	void DvScene::RegisterComponent(DvComponent* pComponent, uint64_t id)
	{
		if (!pComponent)
			return;

		if (id == 0 || GetComponent(id))
		{
			id = getFreeComponentID();
			if (id == 0)
			{
				DV_CORE_ERROR("더이상 DvComponent를 등록할 수 없습니다.");
				return;
			}
		}

		m_Components[id] = pComponent;
		pComponent->SetID(id);
	}

	void DvScene::DeregisterComponent(DvComponent* pComponent)
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

	DvComponent* DvScene::GetComponent(uint64_t id)
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