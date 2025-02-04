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
		void SetName(const std::string& name) { m_Name = name; }
		
	private:
		uint64_t getFreeGameObjectID();

	private:
		std::string m_Name;

		uint64_t m_CurGameObjectID;
		std::unordered_map<uint64_t, GameObject*> m_GameObjects;

		friend class WorldSerializer;

		// 이하 유니티 인터페이스
		// 런타임 데이타 구조체라고 한다.
		// buildIndex, isDirty, isLoaded, name, path, rootCount
		// GetRootGameObjects, IsValid
		// GameObject는 생성시 자동으로 활성화된 Scene에 추가된다. GameObject에 scene이라는 멤버 변수도 있다.
		// 언제부터였는지 모르겠지만 SceneManager란 것이 있다.
		
		// => 현재 내가 구현한 World는 언리얼의 구조에 가깝다.
	};
}