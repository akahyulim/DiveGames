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

		// ���� ����Ƽ �������̽�
		// ��Ÿ�� ����Ÿ ����ü��� �Ѵ�.
		// buildIndex, isDirty, isLoaded, name, path, rootCount
		// GetRootGameObjects, IsValid
		// GameObject�� ������ �ڵ����� Ȱ��ȭ�� Scene�� �߰��ȴ�. GameObject�� scene�̶�� ��� ������ �ִ�.
		// �������Ϳ����� �𸣰����� SceneManager�� ���� �ִ�.
		
		// => ���� ���� ������ World�� �𸮾��� ������ ������.
	};
}