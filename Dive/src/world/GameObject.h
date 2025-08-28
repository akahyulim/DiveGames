#pragma once
#include "core/Object.h"
#include "components/Component.h"

namespace Dive
{
	class World;
	class Transform;

	// https://docs.unity3d.com/ScriptReference/GameObject.html
	class GameObject : public Object
	{
	public:
		GameObject(World* world, const std::string& name);
		~GameObject() override;

		void Destory();

		void Update();
		
		bool IsDestroyed() const { return m_destroyed; }

		void SetTag(const std::string& tag) { m_tag = tag; }
		std::string GetTag() const { return m_tag; }
		bool CompareTag(const std::string& tag) const { return m_tag == tag; }

		void SetActive(bool value);
		bool IsActiveSelf() const { return m_activeSelf; }
		bool IsActiveHierarchy() const { return m_activeHierarchy; }

		template<typename T> T* AddComponent();

		template<typename T> void RemoveComponent();
		void RemoveComponentByType(eComponentType type);

		template<typename T> bool HasComponent() const;
		bool HasComponentByType(eComponentType type) const;

		template<typename T> T* GetComponent() const;
		Component* GetComponentByType(eComponentType type) const;

		Transform* GetTransform() const;

		World* GetWorld() const { return m_world; }

	private:
		void updateActiveInHierarchy(bool parentHierarchy);

	private:
		std::string m_tag = "Untagged";
		// layer

		bool m_destroyed = false;
		bool m_activeSelf = true;
		bool m_activeHierarchy = true;

		World* m_world = nullptr;
		std::unordered_map<eComponentType, std::unique_ptr<Component>> m_components;
	};

	template<typename T>
	T* GameObject::AddComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T는 반드시 Component를 상속하여야 한다.");

		auto type = T::GetComponentType();
		if (!HasComponentByType(type))
			m_components[type] = std::make_unique<T>(this);

		return static_cast<T*>(m_components[type].get());
	}

	template<typename T>
	void GameObject::RemoveComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T는 반드시 Component를 상속하여야 한다.");
		RemoveComponentByType(T::GetComponentType());
	}

	template<typename T>
	bool GameObject::HasComponent() const
	{
		static_assert(std::is_base_of<Component, T>::value, "T는 반드시 Component를 상속하여야 한다.");
		return HasComponentByType(T::GetComponentType());
	}

	template<typename T>
	T* GameObject::GetComponent() const
	{
		static_assert(std::is_base_of<Component, T>::value, "T는 반드시 Component를 상속하여야 한다.");
		return static_cast<T*>(GetComponentByType(T::GetComponentType()));
	}
}
