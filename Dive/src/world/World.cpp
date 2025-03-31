#include "stdafx.h"
#include "World.h"
#include "WorldSerializer.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "core/EventDispatcher.h"
#include "core/UUID.h"

namespace Dive
{
	World::World(const std::string& name)
		: m_Name(name)
		, m_CurGameObjectID(0)
		, m_IsDirty(true)
	{
		DV_SUBSCRIBE_EVENT(Dive::eEventType::WorldModified, DV_EVENT_HANDLER(OnModified));
	}

	World::~World()
	{
		Clear();
	}

	void World::Clear()
	{
		for (auto& [id, gameObject] : m_GameObjects)
		{
			DV_DELETE(gameObject);
		}
		m_GameObjects.clear();
		m_CurGameObjectID = 0;
		m_IsDirty = true;
	}

	void World::Update()
	{
		for (auto& [id, gameObject] : m_GameObjects)
		{
			gameObject->Update();
		}
	}

	GameObject* World::CreateGameObject(const std::string& name)
	{
		return CreateGameObject(UUID(), name);
	}

	GameObject* World::CreateGameObject(UINT64 id, const std::string& name)
	{
		if (m_GameObjects.find(id) != m_GameObjects.end())
		{
			DV_LOG(World, warn, "이미 존재하는 ID: {}", id);
			return nullptr;
		}

		auto gameObject = new GameObject(this, name);
		m_GameObjects[id] = gameObject;
		gameObject->SetID(id);

		return gameObject;
	}

	void World::DeleteGameObject(GameObject* gameObject)
	{
		if (!gameObject || m_GameObjects.empty())
			return;

		DeleteGameObjectByID(gameObject->GetID());
	}

	void World::DeleteGameObjectByID(UINT64 id)
	{
		auto it = m_GameObjects.find(id);
		if (it != m_GameObjects.end())
		{
			for (auto child : it->second->GetTransform()->GetChildren())
			{
				child->SetParent(nullptr);
				DeleteGameObjectByID(child->GetGameObject()->GetID());
			}

			DV_DELETE(it->second);
			m_GameObjects.erase(it);
			m_IsDirty = true;
		}
	}

	GameObject* World::GetGameObjectByID(UINT64 id)
	{
		auto it = m_GameObjects.find(id);
		return it != m_GameObjects.end() ? it->second : nullptr;
	}

	bool World::ExistsGameObject(GameObject* gameObject)
	{
		if (!gameObject)
			return false;

		return ExistsGameObjectByID(gameObject->GetID());
	}

	bool World::ExistsGameObjectByID(UINT64 id)
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

	UINT64 World::GetGameObjectsCount()
	{
		return static_cast<UINT64>(m_GameObjects.size());
	}

	void World::SetName(const std::string& name)
	{
		if (m_Name != name)
		{
			m_Name = name;
			m_IsDirty = true;
		}
	}

	void World::OnModified()
	{
		m_IsDirty = true;
	}
}