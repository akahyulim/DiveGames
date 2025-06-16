#pragma once
#include "Components/Component.h"

namespace Dive
{
	class Transform;

	enum class eLayer : int
	{

	};

	class GameObject
	{
	public:
		GameObject(const std::string& name = "GameObject");
		GameObject(UINT64 instanceID, const std::string& name = "GameObject");
		~GameObject();

		void Update();

		UINT64 GetInstanceID() const { return m_InstanceID; }
		
		void SetName(const std::string& name) { m_Name = name; }
		const std::string& GetName() const { return m_Name; }

		void SetTag(const std::string& tag) { m_Tag = tag; }
		std::string GetTag() const { return m_Tag; }
		bool CompareTag(const std::string& tag) const { return m_Tag == tag; }

		void SetActive(bool value);
		bool IsActiveSelf() const { return m_ActiveSelf; }
		bool IsActiveHierarchy() const { return m_ActiveHierarchy; }

		template<typename T> T* AddComponent();

		template<typename T> void RemoveComponent();
		void RemoveComponentByType(eComponentType type);

		template<typename T> bool HasComponent() const;
		bool HasComponentByType(eComponentType type) const;

		template<typename T> T* GetComponent() const;
		Component* GetComponentByType(eComponentType type) const;

		Transform* GetTransform() const { return m_Transform; }

	private:
		void updateActiveInHierarchy(bool parentHierarchy);

	private:
		UINT64 m_InstanceID;
		std::string m_Name;
		
		std::string m_Tag = "Untagged";
		
		bool m_ActiveSelf = true;
		bool m_ActiveHierarchy = true;

		std::unordered_map<eComponentType, Component*> m_Components;

		// layer
		Transform* m_Transform = nullptr;
	};

	template<typename T>
	T* GameObject::AddComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");

		auto type = T::GetType();
		if (HasComponentByType(type))
		{
			DV_LOG(GameObject, warn, "Component of type {} already exists on GameObject '{}'.", typeid(T).name(), GetName());
			return static_cast<T*>(GetComponentByType(type));
		}

		T* component = new T(this);
		m_Components[type] = component;
		return component;
	}

	template<typename T>
	void GameObject::RemoveComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
		RemoveComponentByType(T::GetType());
	}

	template<typename T>
	bool GameObject::HasComponent() const
	{
		static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
		return HasComponentByType(T::GetType());
	}

	template<typename T>
	T* GameObject::GetComponent() const
	{
		static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
		return static_cast<T*>(GetComponentByType(T::GetType()));
	}
}