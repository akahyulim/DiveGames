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

		const Transform* GetTransform() const { return m_pTransform; }	// �б� �������� ����

		Scene* GetScene() { return m_pScene; }

		template<class T> T* AddComponent();
		template<class T> bool AddComponent(T* pComponent);

		template<class T> bool HasComponent();
		bool HashComponent(size_t typeHash) const;
		
		template<class T> T* GetComponent() const;
		Component* GetComponent(size_t typeHash) const;

		// static
		// Find : �̸����� ã�� �ǵ� �� �� �˾ƺ���.
		// FindGameObjectWithTag : Ȱ��ȭ�� ������Ʈ �迭 ����
		// FindWithTag : ù ��° Ȱ��ȭ�� ���� ������Ʈ ����
		// GetScene : �ڽ��� ���Ե� scene�̶�µ�... �� �� �˾ƺ���.

	private:
		std::string m_Name;
		uint64_t m_ID;

		std::string m_Tag;

		Scene* m_pScene;
		bool m_bActive;
		bool m_bMarkedTarget;
		std::unordered_map<size_t, Component*> m_Components;

		const Transform* const m_pTransform;	// �б� ����
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