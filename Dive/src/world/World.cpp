#include "stdafx.h"
#include "World.h"
#include "GameObject.h"
#include "transforms/Transforms.h"

namespace Dive
{
	World::World(const std::string& name)
	{
		m_Name = name.empty() ? "New World" : name;
	}

	World::~World()
	{
	}

	GameObject World::CreateGameObject(const std::string& name)
	{
		return CreateGameObjectWithUUID(UUID(), name);
	}

	GameObject World::CreateGameObjectWithUUID(UUID uuid, const std::string& name)
	{
		GameObject gameObject = { m_Registry.create(), this };
		gameObject.AddComponent<IDComponent>(uuid);
		auto& tag = gameObject.AddComponent<TagComponent>();
		tag = name.empty() ? "GameObject" : name;
		gameObject.AddComponent<ActiveComponent>();
		gameObject.AddComponent<TransformComponent>();
		
		m_GameObjects[uuid] = gameObject;

		return gameObject;

	}

	void World::RemoveGameObject(GameObject gameObject)
	{
		for (auto& child : Transforms::GetChildren(gameObject))
		{
			RemoveGameObject(child);
		}

		Transforms::RemoveParent(gameObject);
		m_GameObjects.erase(gameObject.GetUUID());
		m_Registry.destroy(gameObject);
	}

	GameObject World::GetGameObjectByName(const std::string& name)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto gameObject : view)
		{
			const TagComponent& tag = view.get<TagComponent>(gameObject);
			if (tag.Tag == name)
				return GameObject{ gameObject, this };
		}

		return {};
	}

	GameObject World::GetGameObjectByUUID(UUID uuid)
	{
		if (m_GameObjects.find(uuid) != m_GameObjects.end())
			return { m_GameObjects.at(uuid), this };

		return {};
	}

	std::vector<GameObject> World::GetAllGameObjects()
	{
		std::vector<GameObject> entities{};
		
		if (!m_GameObjects.empty())
		{
			for (auto& [uuid, gameObject] : m_GameObjects)
			{
				entities.emplace_back(gameObject, this);
			}

		}
		return entities;
	}

	void World::SetName(const std::string& name)
	{
		m_Name = name;
	}
}