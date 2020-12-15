#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class Component;

	// hash_number가 필요하다.
	class GameObject : public Object
	{
		DIVE_OBJECT(GameObject, Object);

	public:
		explicit GameObject(Context* context, std::string name = "");
		~GameObject();

		// serialization

		void Clone();

		void Start();
		void Stop();
		void Update();

		// T의 base가 Component인지 확인이 필요하다.
		// 그런데 Component가 다른 Base를 가진다면 좀 애매해진다.
		template<class T> T* AddComponent();
		template<class T> void RemoveComponent();
		template<class T> T* GetComponent();
		template<class T> bool HasComponent();
		const std::vector<std::shared_ptr<Component>>& GetAllComponents() const { return m_components; }

		// get & set
		std::string GetName() const { return m_name; }
		void SetName(std::string name) { m_name = std::move(name); }

		bool IsActive() const { return m_bActive; }
		void SetActive(bool active) { m_bActive = active; }

		unsigned int GetID() const { return m_ID; }

	private:

	private:
		std::string m_name;
		bool m_bActive;
		unsigned int m_ID;

		std::vector<std::shared_ptr<Component>> m_components;
	};

	template<class T> T* GameObject::AddComponent()
	{
		// 동일한 타입이 이미 존재한다면 생성치말고 찾아서 리턴해야 한다.

		auto component = std::make_shared<T>(m_context, this);
		m_components.emplace_back(std::static_pointer_cast<Component>(component));
		component->OnAwake();

		return component.get();
	}
	
	template<class T> void GameObject::RemoveComponent()
	{
		auto it = m_components.begin();
		for (it; it != m_components.end();)
		{
			if ((*it)->GetTypeHash() == typeid(T).hash_code())
			{
				(*it)->OnRemove();
				(*it).reset();
				it = m_components.erase(it);
				break;
			}
			else
				it++;
		}
	}
	
	template<class T> T* GameObject::GetComponent()
	{
		for (auto& component : m_components)
		{
			if (component->GetTypeHash() == typeid(T).hash_code())
				return static_cast<T*>(component);
		}

		return nullptr;
	}
	
	template<class T> bool GameObject::HasComponent()
	{
		for (auto& component : m_components)
		{
			if (component->GetTypeHash() == typeid(T).hash_code())
				return true;
		}

		return false;
	}
}