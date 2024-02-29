#pragma once
#include "Core/Object.h"
#include "Scene.h"

namespace Dive
{
	class Scene;
	class Component;
	class Transform;

	class GameObject : public Object
	{
	public:
		GameObject(Scene* pScene, const std::string& name = "GameObject");
		~GameObject() override;

		void Update();

		bool IsActive() const { return m_bActive; }
		void SetActive(bool active) { m_bActive = active; }

		void MarkRemoveTarget() { m_bMarkedTarget = true; }
		bool IsRemovedTarget() const { return m_bMarkedTarget; }

		template<class T> T* AddComponent();

		template<class T> bool HasComponent();
		bool HashComponent(size_t typeHash) const;
		
		template<class T> T* GetComponent();
		Component* GetComponent(size_t typeHash) const;

		Transform* GetTransform() const { return m_pTransform; }

	private:
		Scene* m_pScene;
		bool m_bActive;
		bool m_bMarkedTarget;
		std::unordered_map<size_t, Component*> m_Components;

		Transform* m_pTransform;
	};

	template<class T>
	T* GameObject::AddComponent()
	{
		auto pExisted = GetComponent<T>();
		if (pExisted)
			return pExisted;

		auto pNewComponent = new T(this);
		m_Components[T::GetTypeHashStatic()] = static_cast<Component*>(pNewComponent);

		return pNewComponent;
	}

	template<class T>
	bool GameObject::HasComponent()
	{
		auto it = m_Components.find(T::GetTypeHashStatic());
		return it != m_Components.end();
	}

	template<class T>
	T* GameObject::GetComponent()
	{
		auto it = m_Components.find(T::GetTypeHashStatic());
		if (it != m_Components.end())
			return dynamic_cast<T*>(it->second);

		return nullptr;
	}
}