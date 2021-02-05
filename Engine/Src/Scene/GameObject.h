#pragma once
#include "Core/Object.h"
#include "Core/Log.h"
#include "Components/Component.h"

namespace Dive
{
	class Context;
	class Component;
	class Transform;
	class FileStream;

	class GameObject : public Object, public std::enable_shared_from_this<GameObject>
	{
		DIVE_OBJECT(GameObject, Object);

	public:
		explicit GameObject(Context* context, std::string name = "");
		~GameObject();

		void Serialize(FileStream& stream);
		void Deserialize(FileStream& stream, Transform* parent = nullptr);

		GameObject* Clone();

		void Start();
		void Stop();
		void Update();

		template<class T> T* AddComponent();
		Component* AddComponent(eComponentType type, unsigned int id = 0);
		template<class T> void RemoveComponent();
		template<class T> T* GetComponent();
		template<class T> bool HasComponent();
		const std::vector<std::shared_ptr<Component>>& GetAllComponents() const { return m_components; }

		void MarkForDestruction() { m_bPendingDestruction = true; }
		bool IsPendingDestruction() const { return m_bPendingDestruction; }

		// get & set
		std::string GetName() const { return m_name; }
		void SetName(std::string name) { m_name = std::move(name); }
		bool IsActive() const { return m_bActive; }
		void SetActive(bool active) { m_bActive = active; }
		unsigned int GetComponentsCount() const { return static_cast<unsigned int>(m_components.size()); }
		std::shared_ptr<GameObject> GetSharedPtr() { return shared_from_this(); }
		Transform* GetTransform() const { return m_transform; }

		GameObject* GetParentGameObject();
		void SetParentByGameObject(GameObject* newParent);
		void SetParentByTrasnsform(Transform* newParent);

	private:
		GameObject(const GameObject&)				= delete;
		GameObject& operator=(const GameObject&)	= delete;

	private:
		std::string m_name;
		bool m_bActive;
		bool m_bPendingDestruction;

		std::vector<std::shared_ptr<Component>> m_components;
		Transform* m_transform;
	};

	template<class T>
	T* GameObject::AddComponent()
	{
		if (auto component = GetComponent<T>())
		{
			return component;
		}

		auto component = std::make_shared<T>(m_context, this);
		m_components.emplace_back(std::static_pointer_cast<Component>(component));
		component->OnAwake();

		return component.get();
	}
	
	template<class T>
	void GameObject::RemoveComponent()
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
	
	template<class T>
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
	
	template<class T>
	bool GameObject::HasComponent()
	{
		for (auto& component : m_components)
		{
			if (component->GetTypeHash() == typeid(T).hash_code())
				return true;
		}

		return false;
	}
}