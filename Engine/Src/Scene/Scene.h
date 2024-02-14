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

		void RegisterComponent(Component* pComponent, uint64_t id = 0);
		void DeregisterComponent(Component* pComponent);
		void DeregisterComponentByID(uint64_t id);
		Component* GetComponent(uint64_t id);

		 std::string GetName() { return m_Name; }

	private:
		uint64_t getFreeGameObjectID();
		uint64_t getFreeComponentID();
	
	private:
		std::string m_Name;

		std::unordered_map<uint64_t, GameObject*> m_GameObjects;
		std::unordered_map<uint64_t, Component*> m_Components;

		uint64_t m_CurGameObjectID;
		uint64_t m_CurComponentID;

		bool m_bDirty;
	};
}