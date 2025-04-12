#pragma once
#include "core/UUID.h"
#include <entt/entt.hpp>

namespace Dive
{
	class GameObject;

	class World
	{
	public:
		World(const std::string& name = std::string());
		~World();

		GameObject CreateGameObject(const std::string& name = std::string());
		GameObject CreateGameObjectWithUUID(UUID uuid, const std::string& name = std::string());
		void RemoveGameObject(GameObject entity);

		GameObject GetGameObjectByName(const std::string& name);
		GameObject GetGameObjectByUUID(UUID uuid);

		std::vector<GameObject> GetAllGameObjects();

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name);

	private:
	private:
		std::string m_Name;

		entt::registry m_Registry;
		std::unordered_map<UUID, entt::entity> m_GameObjects;

		friend class WorldSerializer;
		friend class GameObject;
	};
}