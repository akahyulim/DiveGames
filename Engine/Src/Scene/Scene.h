#pragma once
#include "Base/Object.h"


namespace Dive
{
	class GameObject;

	// GameObjects를 임의로 정렬할 수 있어야 한다.
	// 그렇지 않으면 Editor의 Hierarchy에서 생성 순서대로 Tree에 출력된다.
	// 아니면 적어도 Editor의 Hierarchy에서는 임의 변경이 가능해야 한다.
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
		
		// 유니티의 함수
		// GetRootGameObjects
		// IsValid

		bool IsLoaded() const { return m_bLoaded; }

	private:
		void eraseGameObject(GameObject* pGameObjct);

	private:
		std::vector<GameObject*> m_GameObjects;
		// camera??

		bool m_bChanged;

		// 유니티의 멤버 변수
		bool m_bLoaded = false;
		// unsigned int rootCount
		// std::string Path
		// buildIndex: 빌드 세팅에서의 인덱스...
	};
}