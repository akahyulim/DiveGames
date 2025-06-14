#pragma once

namespace Dive
{
	class GameObject;

	class World
	{
	public:
		World(const std::string& name);
		~World();

		void Clear();

		void Update();

		GameObject* CreateGameObject(const std::string& name = "GameObject");
		GameObject* CreateGameObjectWithInstanceID(UINT64 instanceID, const std::string& name = "GameObject");
		
		void DeleteGameObject(GameObject* gameObject);
		void DeleteGameObjectByInstanceID(UINT64 instanceID);
		
		GameObject* GetGameObjectByInstanceID(UINT64 instanceID);
		
		bool ExistsGameObject(GameObject* gameObject);
		bool ExistsGameObjectByInstanceID(UINT64 instanceID);
		
		std::vector<GameObject*> GetRootGameObjects();
		std::vector<GameObject*> GetAllGameObjects();
		
		UINT64 GameObjectsCount();

		bool IsEmpty() const { return m_GameObjects.empty(); }

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

	private:
		std::string m_Name{};

		std::unordered_map<UINT64, GameObject*> m_GameObjects;
	};
}