#include "stdafx.h"
#include "EntityManager.h"

namespace Dive
{
	void EntityManager::Clear()
	{
		for (auto& [uuid, entity] : m_Entities)
			m_Registry.destroy(entity);
		m_Entities.clear();
	}

	entt::entity EntityManager::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	entt::entity EntityManager::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		entt::entity entity = m_Registry.create();
		AddComponent<IDComponent>(entity).ID = uuid;
		AddComponent<NameComponent>(entity).Name = name;

		return entity;
	}

	void EntityManager::DestroyEntity(entt::entity entity)
	{
		if (Exists(entity))
		{
			auto id = GetComponent<IDComponent>(entity).ID;
			m_Entities.erase(id);
			m_Registry.destroy(entity);
		}
	}
	
	bool EntityManager::Exists(entt::entity entity) 
	{
		return m_Registry.valid(entity);
	}

	entt::entity EntityManager::GetEntity(UUID uuid) 
	{
		DV_ASSERT(EntityManager, m_Entities.find(uuid) != m_Entities.end());

		return m_Entities.at(uuid);
	}

	std::vector<entt::entity> EntityManager::GetEntities(const std::string& name)
	{
		std::vector<entt::entity> entities;

		auto view = m_Registry.view<NameComponent>();
		for (auto [entity, nc] : view.each())
		{
			if (nc.Name == name)
				entities.emplace_back(entity);
		}

		return entities;
	}

	std::vector<entt::entity> EntityManager::GetAllEntities()
	{
		std::vector<entt::entity> entities;
		entities.reserve(m_Entities.size());

		for (auto& [uuid, entity] : m_Entities)
		{
			entities.emplace_back(entity);
		}

		return entities;
	}
}