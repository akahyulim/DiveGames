#pragma once
#include "Components/Component.h"

namespace Dive
{
	class World;
	class Transform;

	// https://docs.unity3d.com/ScriptReference/GameObject.html
	class GameObject : public std::enable_shared_from_this<GameObject>
	{
	public:
		GameObject(UINT64 instanceID, const std::string& name);
		~GameObject();

		void Destory();

		void Update();

		UINT64 GetInstanceID() const { return m_InstanceID; }
		
		bool IsDestroyed() const { return m_IsDestroyed; }

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

		World* GetWorld() const { return m_World; }

		std::shared_ptr<GameObject> GetSharedPtr() { return shared_from_this(); }

	private:
		void updateActiveInHierarchy(bool parentHierarchy);

	private:
		// 유니티에선 객체 비교용... 
		// 실제로 직렬화/역직렬화를 위해 fileID라는 추가 id가 존재한다.
		// 이는 Object에 선언된 듯 하며 이를 상속하는 모든 객체가 가진다.
		// 즉, Component들도 가진다.
		// 비교용으로 사용하려면 ==도 오버로딩하는 게 맞는 듯 하다.
		UINT64 m_InstanceID;	
		std::string m_Name;

		bool m_IsDestroyed = false;
		
		std::string m_Tag = "Untagged";
		// layer
		
		bool m_ActiveSelf = true;
		bool m_ActiveHierarchy = true;

		World* m_World = nullptr;

		std::unordered_map<eComponentType, Component*> m_Components;

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