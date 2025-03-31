#pragma once
#include "Core/Object.h"
#include "World.h"

namespace Dive
{
	class World;
	class Component;
	class Transform;
	
	class GameObject
	{
	public:
		GameObject(World* world, const std::string& name);
		~GameObject();

		void Update();

		void SetName(const std::string& name);
		const std::string& GetName() const { return m_Name; }

		const std::string& GetTag() const { return m_Tag; }
		void SetTag(const std::string& tag) { m_Tag = tag; }

		void SetID(UINT64 id) { m_ID = id; }
		UINT64 GetID() const { return m_ID; }

		bool IsActive() const { return m_IsActive; }
		void SetActive(bool active) { m_IsActive = active; }

		const Transform* GetTransform() const { return m_Transform; }	// �б� �������� ����

		World* GetWorld() { return m_World; }

		template<class T> T* AddComponent();
		bool AddComponent(Component* component);

		template<class T> bool HasComponent();
		bool HashComponent(size_t typeHash) const;
		
		template<class T> T* GetComponent() const;
		Component* GetComponent(size_t typeHash) const;

		static bool ComponentValidator(GameObject* gameObject, Component* component);

		// static
		// Find : �̸����� ã�� �ǵ� �� �� �˾ƺ���.
		// FindGameObjectWithTag : Ȱ��ȭ�� ������Ʈ �迭 ����
		// FindWithTag : ù ��° Ȱ��ȭ�� ���� ������Ʈ ����
		// GetWorld : �ڽ��� ���Ե� scene�̶�µ�... �� �� �˾ƺ���.

	private:
		std::string m_Name;
		UINT64 m_ID = 0;

		std::string m_Tag = "Untagged";

		World* m_World;
		bool m_IsActive = true;
		std::unordered_map<size_t, Component*> m_Components;

		const Transform* const m_Transform;	// �б� ����
	};

	template<class T>
	T* GameObject::AddComponent()
	{
		auto existed = GetComponent<T>();
		if (existed)
			return existed;
		
		auto component = new T(this);
		m_Components[T::GetTypeHashStatic()] = static_cast<Component*>(component);

		return component;
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