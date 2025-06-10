#pragma once
#include "core/InstanceID.h"
#include "Components.h"

#include <entt/entt.hpp>

namespace Dive
{
	enum class ePrimitiveType
	{
		None = 0,
		Triangle,
		Cube,
		Plane,
		Quad,
		Sphere,
		Capsule,
	};

	class EntityManager
	{
	public:
		EntityManager() = default;
		~EntityManager() = default;

		void Clear();

		entt::entity CreateEntity(const std::string& name = std::string());
		entt::entity CreateEntityWithUUID(InstanceID instanceID, const std::string& name = std::string());
		template<typename T, typename... Args>
		entt::entity CreateEntityWithComponents()
		{
			entt::entity entity = CreateEntity();
			(AddComponent<T>(entity), ..., AddComponent<Args>(entity));
			return entity;
		}

		void DeleteEntity(entt::entity entity);
		
		bool Exists(entt::entity entity);

		entt::entity GetEntity(InstanceID instanceID);
		std::vector<entt::entity> GetEntities(const std::string& name);
		std::vector<entt::entity> GetAllEntities();
		
		std::vector<entt::entity> GetRootEntities();

		template<typename T, typename... Args>
		T& AddComponent(entt::entity entity, Args&... args)
		{
			return m_Registry.emplace<T>(entity, std::forward<Args>(args)...);
		}

		template<typename T>
		void RemoveComponent(entt::entity entity)
		{
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
			DV_ASSERT(EntityManager, Exists(entity));

			auto view = m_Registry.view<T>();
			return view.contains(entity);
		}

		template<typename... Components>
		auto GetEntityView() 
		{
			return m_Registry.view<Components...>();
		}

		InstanceID GetInstanceID(entt::entity entity);
		std::string GetName(entt::entity entity);

		entt::entity CreatePrimitive(ePrimitiveType type);

	private:
	private:
		entt::registry m_Registry;
		std::unordered_map<InstanceID, entt::entity> m_Entities;
	};
}