#pragma once

namespace Dive
{
	class Transform;
	class GameObject;

	// https://docs.unity3d.com/6000.1/Documentation/ScriptReference/SceneManagement.Scene.html
	class World
	{
	public:
		World(const std::string& name);
		~World();

		void Clear();

		void Update();

		GameObject* CreateGameObject(const std::string& name = "");

		void DestroyGameObject(GameObject* gameObject);
		void DestroyGameObject(UINT64 instanceID);
		void QueueDestroy(GameObject* gameObject);
		void QueueDestroy(UINT64 instanceID);
		void FlushDestoryQueue();

		bool HasGameObject(UINT64 instanceID);

		GameObject* FindGameObject(UINT64 instanceID);

		void AttachRoot(GameObject* gameObject);
		void AttachRoot(UINT64 instanceID);
	
		void DetachRoot(GameObject* gameObject);
		void DetachRoot(UINT64 instanceID);

		size_t AllGameObjectCount() const { return m_GameObjectMap.size(); }
		std::vector<GameObject*> GetAllGameObjects();

		size_t RootGameObjectCount() const { return m_RootGameObjects.size(); }
		const std::vector<GameObject*>& GetRootGameObjects() { return m_RootGameObjects; }

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

	private:
		std::string m_Name{};

		std::unordered_map<UINT64, std::unique_ptr<GameObject>> m_GameObjectMap;
		std::vector<GameObject*> m_RootGameObjects;
		std::unordered_set<UINT64> m_DestroyQueue;

		friend class Transform;
	};

	//https://docs.unity3d.com/ScriptReference/SceneManagement.SceneManager.html
	class WorldManager
	{
	public:
		static World* CreateWorld(const std::string& name);
		static World* GetActiveWorld();

	private:
		static std::unique_ptr<World> s_ActiveWorld;	// 일단 단일 World로 관리
	};
}