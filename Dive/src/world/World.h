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

		std::shared_ptr<GameObject> CreateGameObject(const std::string& name = "");
		std::shared_ptr<GameObject> CreateGameObject(UINT64 instanceID, const std::string& name = "");

		void DestroyGameObject(std::shared_ptr<GameObject> gameObject);
		void DestroyGameObject(UINT64 instanceID);
		void QueueDestory(GameObject* gameObject);
		void FlushDestoryQueue();

		bool HasGameObject(UINT64 instanceID);

		std::shared_ptr<GameObject> FindGameObject(UINT64 instanceID);

		void AttachRoot(GameObject* gameObject);
		void DetachRoot(GameObject* gameObject);

		size_t AllGameObjectCount() const { return m_GameObjectMap.size(); }
		std::vector<std::shared_ptr<GameObject>> GetAllGameObjects();

		size_t RootGameObjectCount() const { return m_RootGameObjects.size(); }
		const std::vector<std::shared_ptr<GameObject>>& GetRootGameObjects() { return m_RootGameObjects; }

		void TraverseHierarchy(const std::function<void(GameObject*)>& visit);

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

	private:
		void traverseRecursive(GameObject* gameObject, const std::function<void(GameObject*)>& visit);

	private:
		std::string m_Name{};

		std::unordered_map<UINT64, std::shared_ptr<GameObject>> m_GameObjectMap;
		std::vector<std::shared_ptr<GameObject>> m_RootGameObjects;
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
		static World* s_ActiveWorld;	// 일단 단일 World로 관리
	};
}