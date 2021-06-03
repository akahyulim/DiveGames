#pragma once
#include "Object.h"
#include "Component.h"
#include "Transform.h"
#include <string>
#include <vector>
#include <memory>

namespace Dive
{
	class GameObject : public Object
	{
	public:
		GameObject();
		~GameObject() = default;

		void Update(float deltaTime);

		template<typename T>
		T* AddComponent();

		template<typename T>
		void RemoveComponent();

		template<typename T>
		T* GetComponent();

		Transform* GetTransform() { return m_pTransform; }

		const std::string& GetName() const { return m_name; }
		void SetName(const std::string name) { m_name = name; }

		bool GetActive() const { return m_bActive; }
		void SetActive(bool active) { m_bActive = active; }

	private:
	private:
		// 유니티는 'Object 1'로 시작한다.
		std::string m_name = "Object";
		bool m_bActive = true;

		std::vector<std::shared_ptr<Component>> m_components;

		Transform* m_pTransform;

	};

	template<typename T>
	T* GameObject::AddComponent()
	{
		if (auto component = GetComponent<T>())
		{
			return component;
		}

		// 생성자를 선언해야 한다.
		auto component = std::make_shared<T>();
		m_components.push_back(std::static_pointer_cast<Component>(component));
		// awake가 있다면 호출

		return component.get();
	}

	template<typename T>
	void GameObject::RemoveComponent()
	{
		auto& it = m_components.begin();
		for (it; it != m_components.end();)
		{
			if ((*it)->GetTypeHash() == typeid(T).hash_code())
			{
				// 제거 함수 호출?
				(*it).reset();
				it = m_components.erase(it);
				return;
			}
			else
				it++;
		}
	}

	template<typename T>
	T* GameObject::GetComponent()
	{
		for (auto& component : m_components)
		{
			if (component->GetTypeHash() == typeid(T).hash_code())
			{
				return static_cast<T*>(component.get());
			}
		}

		return nullptr;
	}
}