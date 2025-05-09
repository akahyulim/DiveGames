#pragma once
#include "core/UUID.h"
#include "Components.h"

#include <entt/entt.hpp>

namespace Dive
{
	class EntityManager
	{
	public:
		EntityManager() = default;
		~EntityManager() = default;

		void Clear();

		entt::entity CreateEntity(const std::string& name = std::string());
		entt::entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(entt::entity entity);
		
		bool Exists(entt::entity entity);

		entt::entity GetEntity(UUID uuid);
		std::vector<entt::entity> GetEntities(const std::string& name);
		std::vector<entt::entity> GetAllEntities();

		template<typename T, typename... Args>
		T& AddComponent(entt::entity entity, Args&... args)
		{
			DV_ASSERT(Entity, !HasComponent<T>(entity));
			T& component = m_Registry.emplace<T>(entity, std::forward<Args>(args)...);

			return component;
		}

		template<typename T>
		void RemoveComponent(entt::entity entity)
		{
			DV_ASSERT(Entity, HasComponent<T>(entity));
			m_Registry.remove<T>(entity);
		}

		template<typename T>
		T& GetComponent(entt::entity entity)
		{
			DV_ASSERT(Entity, HasComponent<T>(entity));
			return m_Registry.get<T>(entity);
		}

		template<typename T>
		bool HasComponent(entt::entity entity)
		{
			auto view = m_Registry.view<T>();
			return view.contains(entity);
		}

		template<typename... Components>
		auto GetEntityView() 
		{
			return m_Registry.view<Components...>();
		}

	private:
	private:
		entt::registry m_Registry;
		std::unordered_map<UUID, entt::entity> m_Entities;
	};
}