#pragma once
#include "Base/Object.h"


namespace Dive
{
	class GameObject;

	// GameObjects�� ���Ƿ� ������ �� �־�� �Ѵ�.
	// �׷��� ������ Editor�� Hierarchy���� ���� ������� Tree�� ��µȴ�.
	// �ƴϸ� ��� Editor�� Hierarchy������ ���� ������ �����ؾ� �Ѵ�.
	class Scene : public Object
	{
	public:
		Scene();
		Scene(const std::string& sceneName);
		~Scene();

		void Update(float deltaTime);

		void Clear();

		bool SaveToFile(const std::string& filepath);
		bool LoadFromFile(const std::string& filepath);

		GameObject* CreateGameObject(bool active = true);
		GameObject* GetGameObjectByName(const std::string& name);
		GameObject* GetGameObjectByID(unsigned int id);
		void RemoveGameObject(GameObject* pGameObjct);

		const std::vector<GameObject*>& GetAllGameObjects() const { return m_GameObjects; }
		std::vector<GameObject*> GetRootGameObjects() const;

		unsigned int GetGameObjectCount() const { return static_cast<unsigned int>(m_GameObjects.size()); }

		bool IsDirty() { return m_bChanged; }
		
		// ����Ƽ�� �Լ�
		// GetRootGameObjects
		// IsValid

		bool IsLoaded() const { return m_bLoaded; }

	private:
		void eraseGameObject(GameObject* pGameObjct);

	private:
		std::vector<GameObject*> m_GameObjects;
		// camera??

		bool m_bChanged;

		// ����Ƽ�� ��� ����
		bool m_bLoaded = false;
		// unsigned int rootCount
		// std::string Path
		// buildIndex: ���� ���ÿ����� �ε���...
	};
}