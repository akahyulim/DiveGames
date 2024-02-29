#pragma once

namespace Dive
{
	class GameObject;
	class Component;

	class Scene
	{
	public:
		Scene(const std::string& name = "Untitled");
		~Scene();

		void Clear();
		
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

		 std::string GetName() { return m_Name; }

	private:
		uint64_t getFreeGameObjectID();
	
	private:
		std::string m_Name;

		std::unordered_map<uint64_t, GameObject*> m_GameObjects;

		uint64_t m_CurGameObjectID;

		bool m_bDirty;
	};
}