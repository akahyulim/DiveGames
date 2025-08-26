#pragma once
#include "core/EventDispatcher.h"

namespace Dive
{
	class Camera;
	class Light;
	class Transform;
	class MeshRenderer;
	class GameObject;

	// https://docs.unity3d.com/6000.1/Documentation/ScriptReference/SceneManagement.Scene.html
	class World
	{
	public:
		World(const std::string& name);
		~World();

		void Clear();

		void Update();

		void CullAndSort(Camera* camera);

		GameObject* CreateGameObject(const std::string& name = "");

		void DestroyGameObject(GameObject* gameObject);
		void DestroyGameObject(uint64_t instanceID);
		void QueueDestroy(GameObject* gameObject);
		void QueueDestroy(uint64_t instanceID);
		void FlushDestoryQueue();

		bool HasGameObject(uint64_t instanceID);

		GameObject* FindGameObject(uint64_t instanceID);

		void AttachRoot(GameObject* gameObject);
		void AttachRoot(uint64_t instanceID);
	
		void DetachRoot(GameObject* gameObject);
		void DetachRoot(uint64_t instanceID);

		size_t AllGameObjectCount() const { return m_gameObjectMap.size(); }
		std::vector<GameObject*> GetAllGameObjects();

		size_t RootGameObjectCount() const { return m_rootGameObjects.size(); }
		const std::vector<GameObject*>& GetRootGameObjects() { return m_rootGameObjects; }

		const std::vector<Light*>& GetLights() const { return m_lights; }

		const std::vector<MeshRenderer*>& GetTransparentMeshRenderers() const { return m_transparentMeshRenderers; }
		const std::vector<MeshRenderer*>& GetOpaqueMeshRenderers() const { return m_opaqueMeshRenderers; }

		std::string GetName() const { return m_name; }
		void SetName(const std::string& name) { m_name = name; }

	private:
		std::string m_name{};

		std::unordered_map<uint64_t, std::unique_ptr<GameObject>> m_gameObjectMap;
		std::vector<GameObject*> m_rootGameObjects;
		std::unordered_set<uint64_t> m_destroyQueue;

		std::vector<Light*> m_lights;

		std::vector<MeshRenderer*> m_transparentMeshRenderers;
		std::vector<MeshRenderer*> m_opaqueMeshRenderers;

		friend class Transform;
	};

	//https://docs.unity3d.com/ScriptReference/SceneManagement.SceneManager.html
	class WorldManager
	{
	public:
		static bool Initialize();

		static World* CreateWorld(const std::string& name);
		static void Clear();

		static void OnUpdate(EventData data);

		static World* GetActiveWorld();

	private:
		static std::unique_ptr<World> s_activeWorld;	// 일단 단일 World로 관리
	};
}
