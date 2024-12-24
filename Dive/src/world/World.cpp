#include "stdafx.h"
#include "World.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "core/CoreDefs.h"

namespace Dive
{
	static constexpr uint64_t FIRST_ID = 0x1;
	static constexpr uint64_t LAST_ID = 0xffffffffffffffff;

	World::World()
		: m_Name("empty_world")
		, m_CurGameObjectID(FIRST_ID)
		, m_bDirty(false)
	{
	}

	World::World(const std::string& name)
		: m_Name(name)
		, m_CurGameObjectID(FIRST_ID)
		, m_bDirty(false)
	{
	}

	World::~World()
	{
		Clear();
	}

	void World::Clear()
	{
		for (auto& [id, pGameObject] : m_GameObjects)
		{
			DV_DELETE(pGameObject);
		}
		m_GameObjects.clear();
		m_CurGameObjectID = FIRST_ID;
	}

	void World::Tick()
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
			{
				++it;
			}
		}

		for (auto& [id, pGameObject] : m_GameObjects)
		{
			pGameObject->Update();
		}
	}

	void World::SaveToFile(const std::filesystem::path& filepath)
	{
		YAML::Node config;

		config["Name"] = m_Name;

		YAML::Node gameObjectsNode;
		for (auto& [Id, pGameObject] : m_GameObjects)
		{
			YAML::Node gameObjectNode;
			pGameObject->SaveToYaml(gameObjectNode);
			gameObjectsNode.push_back(gameObjectNode);
		}

		config["GameObjects"] = gameObjectsNode;
		
		std::filesystem::path file = filepath / (m_Name + ".world");
		std::ofstream fout(file);
		fout << config;

		m_Path = filepath;
	}
	
	void World::LoadFromFile(const std::filesystem::path& filepath)
	{
		YAML::Node config = YAML::LoadFile(filepath.string());

		m_Name = config["Name"].as<std::string>();

		m_GameObjects.clear();
		for (const auto& gameObjectNode : config["GameObjects"])
		{
			auto pGameObject = new GameObject(this);
			pGameObject->LoadFromYaml(gameObjectNode);
			m_GameObjects[pGameObject->GetID()] = pGameObject;
		}

		if (m_Path != filepath)
			m_Path = filepath;
	}

	GameObject* World::CreateGameObject(const std::string& name)
	{
		auto id = getFreeGameObjectID();
		if (id == 0)
		{
			DV_LOG(World, err, "더이상 새로운 GameObjecrt를 생성할 수 없습니다.");
			return nullptr;
		}

		auto pNewGameObject = new GameObject(this, name);
		m_GameObjects[id] = pNewGameObject;
		pNewGameObject->SetID(id);

		return pNewGameObject;
	}

	void World::RemoveGameObject(GameObject* pGameObject)
	{
		if (!pGameObject || m_GameObjects.empty())
			return;

		RemoveGameObjectByID(pGameObject->GetID());
	}

	void World::RemoveGameObjectByID(uint64_t id)
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

	GameObject* World::GetGameObjectByID(uint64_t id)
	{
		auto it = m_GameObjects.find(id);
		return it != m_GameObjects.end() ? it->second : nullptr;
	}

	bool World::ExistsGameObject(GameObject* pGameObject)
	{
		if (!pGameObject)
			return false;

		return ExistsGameObjectByID(pGameObject->GetID());
	}

	bool World::ExistsGameObjectByID(uint64_t id)
	{
		auto it = m_GameObjects.find(id);
		return it != m_GameObjects.end();
	}

	std::vector<GameObject*> World::GetRootGameObjects()
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

	std::vector<GameObject*> World::GetAllGameObjects()
	{
		std::vector<GameObject*> allGameObjects;

		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end(); ++it)
			allGameObjects.emplace_back(it->second);

		return allGameObjects;
	}

	uint64_t World::GetGameObjectsCount()
	{
		return static_cast<uint64_t>(m_GameObjects.size());
	}

	uint64_t World::getFreeGameObjectID()
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