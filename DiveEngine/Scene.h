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

		// 이제 이건 지워져야 한다.
		// 그런데 사용되는 곳이 꽤 될 거다.
		static Scene& GetGlobalScene()
		{
			static Scene scene;
			return scene;
		}

		bool IsDirty() { return m_bDirty; }
		
		// 유니티의 함수
		// GetRootGameObjects
		// IsValid

	private:
		void eraseGameObject(GameObject* gameObject);

	private:
		std::vector<GameObject*> m_GameObjects;
		// camera??

		bool m_bDirty;

		// 유니티의 멤버 변수
		// bLoaded
		// unsigned int rootCount
		// std::string Path
		// buildIndex: 빌드 세팅에서의 인덱스...
	};
}