#pragma once
#include "core/UUID.h"
#include <entt/entt.hpp>

namespace Dive
{
	struct TransformComponent;
	class Entity;

	class World
	{
	public:
		World(const std::string& name = std::string());
		~World();

		void Clear();

		void EditorTick(float deltaTime);
		void RuntimeTick(float deltaTime);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void RemoveEntity(Entity entity);

		Entity GetEntityByName(const std::string& name);
		Entity GetEntityByUUID(UUID uuid);

		std::vector<Entity> GetAllEntities();

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }
		
		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:
	private:
		std::string m_Name;

		entt::registry m_Registry;
		std::unordered_map<UUID, entt::entity> m_Entities;
	
		bool m_Dirty = true;

		friend class WorldSerializer;
		friend class Entity;
	};
}