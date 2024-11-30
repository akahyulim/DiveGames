#pragma once

namespace Dive
{
	class GameObject;
	class Component;

	// �𸮾󿡼� �⺻������ Game�� Editor �� ���� Ÿ������ ���еȴ�.
	// �׸��� CreateWorld��� Static �޼��带 �����ϸ�, ���ο��� GEngine�� ȣ���� Engine�� ����ȴ�.
	// ����ȭ�� ��� ���带 �����ϴ� ��κ��� ���ҽ�(��, �ؽ��� ��)��� �Բ� ��Ű¡�ȴ�.

	// �ϴ��� Resource�� ��ӽ�Ű�� �ʾҴ�.
	// ���� ����� �����Ѵٸ� �ϰ��� ������ ���� 
	// ResourceManager�� ���ְ� Resource Ÿ�Ժ� �Ŵ����� ����� ����ؾ� �Ѵ�.
	class Scene
	{
	public:
		Scene();
		Scene(int index, const std::string& name = "Untitled");	// �𸮾󿡼� ���� �ؽ÷� �������� �ʴ´�. �׷��� �̸������� guid�� �ʿ��ϴ�..
		~Scene();

		void Clear();

		void Update();

		bool LoadFromFile(const std::string& fileName);
		bool SaveToFile(const std::string& fileName);
		
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
		bool IsLoaded() const { return m_bLoaded; }

		const std::string GetName() const { return m_Name; }
		void Rename(const std::string& newName) { m_Name = newName; }

		const std::string GetPath() const { return m_Path; }

		int GetIndex() const { return m_Index; }

	private:
		//Scene();
		//~Scene();

		uint64_t getFreeGameObjectID();
	
	private:
		static uint32_t s_WorldCounter;

		std::string m_Name;
		std::string m_Path;

		std::unordered_map<uint64_t, GameObject*> m_GameObjects;

		uint64_t m_CurGameObjectID;

		bool m_bDirty;

		bool m_bLoaded;

		int m_Index;
	};
}