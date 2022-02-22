#pragma once
#include "GameObject.h"

namespace Dive
{
	class Scene
	{
	public:
		Scene(const std::string& name = std::string());
		~Scene();

		// App의 Run에서 직접 호출한다.
		void UpdateRuntime(float elapsedTime);
		// Editor's Camera를 전달한다.
		//void UpdateEditor(float elpasedTime);

		void Clear();

		GameObject* CreateGameObject(const std::string& name = std::string());
		GameObject* CreateGameObject(unsigned long long id, const std::string& name = std::string());
		void RemoveGameObject(unsigned long long id);
		void RemoveGameObject(GameObject* pTarget);
		GameObject* GetGameObject(unsigned long long id);

		std::vector<GameObject*> GetRoots();
		std::vector<GameObject*>& GetGameObjects() { return m_GameObjects; }
		unsigned int GetGameObjectCount() const { return static_cast<unsigned int>(m_GameObjects.size()); }

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

	private:
	private:
		std::string m_Name;
		std::vector<GameObject*> m_GameObjects;
		bool m_bDirty = false;
	};
}