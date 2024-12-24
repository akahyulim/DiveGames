#pragma once
#include "Core/Object.h"
#include "World.h"

namespace Dive
{
	class World;
	class Component;

	class GameObject
	{
	public:
		GameObject(World* pWorld, const std::string& name = "GameObject");
		~GameObject();

		void Update();

		void SaveToYaml(YAML::Node& config);
		void LoadFromYaml(const YAML::Node& config);

		void SetName(const std::string& name) { m_Name = name; }
		const std::string& GetName() const { return m_Name; }
		
		void SetID(uint64_t id) { m_ID = id; }
		uint64_t GetID() const { return m_ID; }

		bool IsActive() const { return m_bActive; }
		void SetActive(bool active) { m_bActive = active; }

		void MarkRemoveTarget() { m_bMarkedTarget = true; }
		bool IsRemovedTarget() const { return m_bMarkedTarget; }

		template<class T> T* AddComponent();
		template<class T> bool AddComponent(T* pComponent);

		template<class T> bool HasComponent();
		bool HashComponent(size_t typeHash) const;
		
		template<class T> T* GetComponent() const;
		Component* GetComponent(size_t typeHash) const;

	private:
		void updateTransform();

	private:
		std::string m_Name;
		uint64_t m_ID;

		World* m_pWorld;
		bool m_bActive;
		bool m_bMarkedTarget;
		std::unordered_map<size_t, Component*> m_Components;

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