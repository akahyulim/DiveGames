#pragma once
#include "Core/Object.h"
#include "Scene.h"

namespace Dive
{
	class Scene;
	class Component;
	class Transform;
	
	class GameObject
	{
	public:
		GameObject(Scene* pScene, const std::string& name = "GameObject");
		~GameObject();

		void Update();

		void SetName(const std::string& name);
		const std::string& GetName() const { return m_Name; }

		const std::string& GetTag() const { return m_Tag; }
		void SetTag(const std::string& tag) { m_Tag = tag; }

		void SetID(uint64_t id) { m_ID = id; }
		uint64_t GetID() const { return m_ID; }

		bool IsActive() const { return m_bActive; }
		void SetActive(bool active) { m_bActive = active; }

		void MarkRemoveTarget() { m_bMarkedTarget = true; }
		bool IsRemovedTarget() const { return m_bMarkedTarget; }

		const Transform* GetTransform() const { return m_pTransform; }	// 읽기 전용으로 전달

		Scene* GetScene() { return m_pScene; }

		template<class T> T* AddComponent();
		template<class T> bool AddComponent(T* pComponent);

		template<class T> bool HasComponent();
		bool HashComponent(size_t typeHash) const;
		
		template<class T> T* GetComponent() const;
		Component* GetComponent(size_t typeHash) const;

		// static
		// Find : 이름으로 찾는 건데 좀 더 알아보자.
		// FindGameObjectWithTag : 활성화됨 오브젝트 배열 리턴
		// FindWithTag : 첫 번째 활성화된 게임 오브젝트 리턴
		// GetScene : 자신이 포함된 scene이라는데... 좀 더 알아보자.

	private:
		std::string m_Name;
		uint64_t m_ID;

		std::string m_Tag;

		Scene* m_pScene;
		bool m_bActive;
		bool m_bMarkedTarget;
		std::unordered_map<size_t, Component*> m_Components;

		const Transform* const m_pTransform;	// 읽기 전용
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
	bool GameObject::AddComponent(T* pComponent)
	{
		if (!pComponent)
			return false;

		auto pExisted = GetComponent<T>();
		if (pExisted)
			return false;

		m_Components[T::GetTypeHashStatic()] = static_cast<Component*>(pComponent);
		pComponent->SetGameObject(this);
		return true;
	}

	template<class T>
	bool GameObject::HasComponent()
	{
		auto it = m_Components.find(T::GetTypeHashStatic());
		return it != m_Components.end();
	}

	template<class T>
	T* GameObject::GetComponent() const
	{
		auto it = m_Components.find(T::GetTypeHashStatic());
		if (it != m_Components.end())
			return dynamic_cast<T*>(it->second);

		return nullptr;
	}
}
