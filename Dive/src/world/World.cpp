#include "stdafx.h"
#include "World.h"
#include "GameObject.h"
#include "transforms/Transforms.h"
#include "transforms/Transforms.h"
#include "rendering/Renderer.h"

namespace Dive
{
	World::World(const std::string& name)
	{
		m_Name = name.empty() ? "New World" : name;
	}

	World::~World()
	{
	}

	void World::EditorTick(float deltaTime)
	{
		{
			std::vector<TransformComponent*> transformList;
			auto view = m_Registry.view<TransformComponent>();
			for (auto entity : view)
			{
				auto& transformComponent = view.get<TransformComponent>(entity);
				transformList.push_back(&transformComponent);
			}
			Transforms::Tick(transformList);

		}

		Renderer::RenderScene(m_Registry);
	}

	void World::RuntimeTick(float deltaTime)
	{
		// 일단 pause, stepFrame(최초 프레임) 확인
		{
			// Script
			{

			}

			// Renderer
			{
				Renderer::RenderScene(m_Registry);
			}
		}
	}

	GameObject World::CreateGameObject(const std::string& name)
	{
		return CreateGameObjectWithUUID(UUID(), name);
	}

	GameObject World::CreateGameObjectWithUUID(UUID uuid, const std::string& name)
	{
		GameObject gameObject = { m_Registry.create(), this };
		gameObject.AddComponent<IDComponent>(uuid);
		auto& nc = gameObject.AddComponent<NameComponent>();
		nc.Name = name.empty() ? "GameObject" : name;
		gameObject.AddComponent<ActiveComponent>();
		gameObject.AddComponent<TransformComponent>();
		gameObject.AddComponent<TagComponent>();
		
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
		auto view = m_Registry.view<NameComponent>();
		for (auto entity : view)
		{
			const NameComponent& tag = view.get<NameComponent>(entity);
			if (tag.Name == name)
				return GameObject{ entity, this };
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