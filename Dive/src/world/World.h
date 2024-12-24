#pragma once

namespace Dive
{
	class GameObject;

	class World
	{
	public:
		World();
		World(const std::string& name);
		~World();

		void Clear();

		void Tick();

		void SaveToFile(const std::filesystem::path& filepath);
		void LoadFromFile(const std::filesystem::path& filepath);

		GameObject* CreateGameObject(const std::string& name = "GameObject");
		void RemoveGameObject(GameObject* pGameObject);
		void RemoveGameObjectByID(uint64_t id);
		GameObject* GetGameObjectByID(uint64_t id);
		bool ExistsGameObject(GameObject* pGameObject);
		bool ExistsGameObjectByID(uint64_t id);
		std::vector<GameObject*> GetRootGameObjects();
		std::vector<GameObject*> GetAllGameObjects();
		uint64_t GetGameObjectsCount();

		bool IsEmpty() const { return m_GameObjects.empty(); }

		std::string GetName() const { return m_Name; }
		void Rename(const std::string& name) { m_Name = name; }

		std::filesystem::path GetPath() const { return m_Path; }

	private:
		uint64_t getFreeGameObjectID();

	private:
		std::string m_Name;
		std::filesystem::path m_Path;

		uint64_t m_CurGameObjectID;
		std::unordered_map<uint64_t, GameObject*> m_GameObjects;

		bool m_bDirty;
	};
}