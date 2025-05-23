#include "stdafx.h"
#include "EntityManager.h"
#include "core/CoreDefs.h"
#include "core/EventDispatcher.h"

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
		m_Entities[uuid] = entity;

		return entity;
	}

	void EntityManager::DeleteEntity(entt::entity entity)
	{
		DV_ASSERT(EntityManager, Exists(entity));

		auto id = GetComponent<IDComponent>(entity).ID;

		DV_LOG(EntityManager, debug, "Delete {0}: entity = {1}, id = {2}", GetComponent<NameComponent>(entity).Name, (uint32_t)entity, (UINT64)id);

		m_Entities.erase(id);
		m_Registry.destroy(entity);
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

	// 정렬되지 않은 상태로 벡터에 저장된다.
	std::vector<entt::entity> EntityManager::GetRootEntities()
	{
		std::vector<entt::entity> roots;

		for (auto& [uuid, entity] : m_Entities)
		{
			if (!HasComponent<ParentComponent>(entity))
			{
				roots.emplace_back(entity);
			}
		}

		return roots;
	}

	UUID EntityManager::GetUUID(entt::entity entity)
	{
		DV_ASSERT(EntityManager, Exists(entity));

		if(!HasComponent<IDComponent>(entity))
			return 0;

		return GetComponent<IDComponent>(entity).ID;
	}
	
	std::string EntityManager::GetName(entt::entity entity)
	{
		DV_ASSERT(EntityManager, Exists(entity));

		if(!HasComponent<NameComponent>(entity))
			return {};

		return GetComponent<NameComponent>(entity).Name;
	}
}