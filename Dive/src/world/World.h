#pragma once
#include "core/EventDispatcher.h"

namespace Dive
{
	struct Ray;
	struct RaycastHit;

	class Camera;
	class Light;
	class Transform;
	class MeshRenderer;
	class GameObject;

	// https://docs.unity3d.com/6000.1/Documentation/ScriptReference/SceneManagement.Scene.html
	class World
	{
	public:
		World() = default;
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

		bool Raycast(const Ray& ray, RaycastHit* outHit);

		void AttachRoot(GameObject* gameObject);
		void AttachRoot(uint64_t instanceID);
	
		void DetachRoot(GameObject* gameObject);
		void DetachRoot(uint64_t instanceID);

		size_t AllGameObjectCount() const { return m_GameObjectMap.size(); }
		std::vector<GameObject*> GetAllGameObjects();

		size_t RootGameObjectCount() const { return m_RootGameObjects.size(); }
		const std::vector<GameObject*>& GetRootGameObjects() { return m_RootGameObjects; }

		const std::vector<Light*>& GetLights() const { return m_Lights; }

		const std::vector<MeshRenderer*>& GetTransparentMeshRenderers() const { return m_Transparents; }
		const std::vector<MeshRenderer*>& GetOpaqueMeshRenderers() const { return m_Opaques; }

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

	private:
		std::string m_Name{};

		std::unordered_map<uint64_t, GameObject*> m_GameObjectMap;
		std::vector<GameObject*> m_RootGameObjects;
		std::unordered_set<uint64_t> m_DestroyQueue;

		std::vector<Light*> m_Lights;

		std::vector<MeshRenderer*> m_Transparents;
		std::vector<MeshRenderer*> m_Opaques;

		// 메인 카메라 정도는 관리했으면 좋겠다.

		friend class Transform;
	};
}
