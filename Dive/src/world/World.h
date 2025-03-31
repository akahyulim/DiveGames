#pragma once
#include "core/CoreDefs.h"

namespace Dive
{
	class GameObject;

	enum class eRenderableType
	{
		Unkonw,
		Camera,
		StaticModel,
		SkinnedModel,
	};

	// ����Ƽ������ �������̽��� ����
	// ���� World�� Editor�� ������ �ʿ䰡 ������.
	// dirty�� ���� filePath���� ���� �����ϴ� ���� ���ƺ��δ�.
	class World
	{
	public:
		World(const std::string& name = "NewWorld");
		~World();

		void Clear();

		void Update();

		GameObject* CreateGameObject(const std::string& name = "GameObject");
		GameObject* CreateGameObject(UINT64 id, const std::string& name = "GameObject");
		void DeleteGameObject(GameObject* gameObject);
		void DeleteGameObjectByID(UINT64 id);
		GameObject* GetGameObjectByID(UINT64 id);
		bool ExistsGameObject(GameObject* gameObject);
		bool ExistsGameObjectByID(UINT64 id);
		std::vector<GameObject*> GetRootGameObjects();
		std::vector<GameObject*> GetAllGameObjects();
		UINT64 GetGameObjectsCount();

		bool IsEmpty() const { return m_GameObjects.empty(); }

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name);

		std::filesystem::path GetFilePath() const { return m_FilePath; }

		bool IsDirty() const { return m_IsDirty; }

		void OnModified();

	private:
		std::string m_Name;
		std::filesystem::path m_FilePath;

		UINT64 m_CurGameObjectID;
		std::unordered_map<UINT64, GameObject*> m_GameObjects;
		std::unordered_map<eRenderableType, std::vector<GameObject*>> m_Renderables;

		bool m_IsDirty;

		friend class WorldSerializer;

		// ���� ����Ƽ �������̽�
		// ��Ÿ�� ����Ÿ ����ü��� �Ѵ�.
		// buildIndex, isDirty, isLoaded, name, path, rootCount
		// GetRootGameObjects, IsValid
		// GameObject�� ������ �ڵ����� Ȱ��ȭ�� World�� �߰��ȴ�. GameObject�� scene�̶�� ��� ������ �ִ�.
		// �������Ϳ����� �𸣰����� WorldManager�� ���� �ִ�.
		
		// => ���� ���� ������ World�� �𸮾��� ������ ������.
	};
}