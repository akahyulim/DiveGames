#pragma once
#include "Object.h"
#include <vector>
#include <string>
#include <memory>

namespace dive
{
	class GameObject;

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
		void RemoveGameObject(GameObject* gameObject);

		const std::vector<GameObject*>& GetAllGameObjects() const { return m_GameObjects; }
		std::vector<GameObject*> GetRootGameObjects() const;

		unsigned int GetGameObjectCount() const { return static_cast<unsigned int>(m_GameObjects.size()); }

		// ���� �̰� �������� �Ѵ�.
		// �׷��� ���Ǵ� ���� �� �� �Ŵ�.
		static Scene& GetGlobalScene()
		{
			static Scene scene;
			return scene;
		}

		bool IsDirty() { return m_bDirty; }
		
		// ����Ƽ�� �Լ�
		// GetRootGameObjects
		// IsValid

		bool IsLoaded() const { return m_bLoaded; }

	private:
		void eraseGameObject(GameObject* gameObject);

	private:
		std::vector<GameObject*> m_GameObjects;
		// camera??

		bool m_bDirty;

		// ����Ƽ�� ��� ����
		bool m_bLoaded = false;
		// unsigned int rootCount
		// std::string Path
		// buildIndex: ���� ���ÿ����� �ε���...
	};
}