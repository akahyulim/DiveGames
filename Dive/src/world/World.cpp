#include "stdafx.h"
#include "World.h"
#include "Entity.h"
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
		Clear();
	}

	void World::Clear()
	{
		m_Registry.clear();
		m_Entities.clear();
		m_Dirty = true;
	}

	void World::EditorTick(float deltaTime)
	{
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

	Entity World::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity World::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		auto& nc = entity.AddComponent<NameComponent>();
		nc.Name = name.empty() ? "Entity" : name;
		entity.AddComponent<ActiveComponent>();
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>();
		
		m_Entities[uuid] = entity;

		m_Dirty = true;

		return entity;

	}

	void World::RemoveEntity(Entity entity)
	{
		for (auto& child : Transforms::GetChildren(entity))
		{
			RemoveEntity(child);
		}
		Transforms::RemoveParent(entity);
		
		m_Entities.erase(entity.GetUUID());
		m_Registry.destroy(entity);

		m_Dirty = true;
	}

	Entity World::GetEntityByName(const std::string& name)
	{
		auto view = m_Registry.view<NameComponent>();
		for (auto entity : view)
		{
			const NameComponent& tag = view.get<NameComponent>(entity);
			if (tag.Name == name)
				return Entity{ entity, this };
		}

		return {};
	}

	Entity World::GetEntityByUUID(UUID uuid)
	{
		if (m_Entities.find(uuid) != m_Entities.end())
			return { m_Entities.at(uuid), this };

		return {};
	}

	std::vector<Entity> World::GetAllEntities()
	{
		std::vector<Entity> gameObjects;
		gameObjects.reserve(m_Entities.size());
		
		if (!m_Entities.empty())
		{
			for (auto& [uuid, entity] : m_Entities)
			{
				gameObjects.emplace_back(entity, this);
			}

		}
		return gameObjects;
	}
}