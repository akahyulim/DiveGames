#pragma once
#include "GameObject.h"
#include "Component/Component.h"

namespace Dive
{
	class Scene
	{
	public:
		Scene(const std::string& name = std::string());
		virtual ~Scene();

		// App의 Run에서 직접 호출한다.
		// Event를 받게 할 수 있다.
		void Update(float elapsedTime);

		void Clear();

		GameObject* CreateGameObject(const std::string& name = std::string(), unsigned long long id = 0);
		void RemoveGameObject(unsigned long long id);
		void RemoveGameObject(GameObject* pTarget);
		GameObject* GetGameObject(unsigned long long id);

		std::vector<GameObject*> GetRoots();
		std::vector<GameObject*>& GetGameObjects() { return m_GameObjects; }	// 이것두 const??
		unsigned int GetGameObjectCount() const { return static_cast<unsigned int>(m_GameObjects.size()); }

		std::vector<Component*> GetComponents(eComponentType type);
		Component* GetComponent(eComponentType type, unsigned long long id);

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		void OnGameObjectModify(const Event& e);

	private:
	private:
		std::string m_Name;
		bool m_bDirty = false;


		std::vector<GameObject*> m_GameObjects;
		std::unordered_map<eComponentType, std::vector<Component*>> m_Components;
	};
}