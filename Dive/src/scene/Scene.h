#pragma once

namespace Dive
{
	class GameObject;

	// ����Ƽ������ �������̽��� ����
	// ���� Scene�� Editor�� ������ �ʿ䰡 ������.
	// dirty�� ���� filePath���� ���� �����ϴ� ���� ���ƺ��δ�.
	class Scene
	{
	public:
		Scene(const std::string& name = "NewScene");
		~Scene();

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
		void SetName(const std::string& name);

		std::filesystem::path GetFilePath() const { return m_FilePath; }

		bool IsDirty() const { return m_bDirty; }

		void OnModified();
		
	private:
		uint64_t getFreeGameObjectID();

	private:
		std::string m_Name;
		std::filesystem::path m_FilePath;

		uint64_t m_CurGameObjectID;
		std::unordered_map<uint64_t, GameObject*> m_GameObjects;

		bool m_bDirty;

		friend class SceneSerializer;

		// ���� ����Ƽ �������̽�
		// ��Ÿ�� ����Ÿ ����ü��� �Ѵ�.
		// buildIndex, isDirty, isLoaded, name, path, rootCount
		// GetRootGameObjects, IsValid
		// GameObject�� ������ �ڵ����� Ȱ��ȭ�� Scene�� �߰��ȴ�. GameObject�� scene�̶�� ��� ������ �ִ�.
		// �������Ϳ����� �𸣰����� SceneManager�� ���� �ִ�.
		
		// => ���� ���� ������ Scene�� �𸮾��� ������ ������.
	};
}