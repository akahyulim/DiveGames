#pragma once
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Component/MeshRenderer.h"
#include "Component/StaticMeshRenderer.h"
#include "Component/Camera.h"
#include "Component/Light.h"
#include "../Core/Object.h"
#include "../Core/Event.h"
#include <string>
#include <vector>

namespace DiveEngine
{
	class Scene;
	class FileStream;

	enum class ePrimitiveType
	{
		Plane,
		Cube,
		Sphere,
		Capsule,
		Cylinder
	};

	class GameObject : public Object
	{
	public:
		GameObject(Scene* scene);
		~GameObject();

		void Serialize(FileStream* pFileStream);
		void Deserialize(FileStream* pFileStream, Transform* pParentTransform = nullptr);

		void Update(float deltaTime);

		GameObject* Duplicate();

		template<typename T>
		T* AddComponent();
		Component* AddComponent(unsigned int typeHash, unsigned int id = 0);

		template<typename T>
		void RemoveComponent();
			
		template<typename T>
		T* GetComponent();

		Transform* GetTransform() { return m_pTransform; }

		bool IsActive() const { return m_bActive; }
		void SetActive(bool active) { m_bActive = active; }

		unsigned int GetComponentCount() const { return static_cast<unsigned int>(m_Components.size()); }

		Scene* GetScene() { return m_pScene; }

		// 이름을 바꾸자
		void MarkForDestruction() { m_bDestructionPending = true; }
		bool IsPendingDestruction() const { return m_bDestructionPending; }

		static GameObject* CreatePrimitive(ePrimitiveType type, Scene* pScene);

	private:
	private:
		Scene* m_pScene = nullptr;
		bool m_bActive = true;	// root false면 자식들도 안보여야 하지 않을까?

		std::vector<Component*> m_Components;

		Transform* m_pTransform = nullptr;

		// 이름을 바꾸자
		bool m_bDestructionPending = false;

		// unity
		// std::string m_Tag;
		// unsigned int m_Layer;

	};

	template<typename T>
	T* GameObject::AddComponent()
	{
		if (auto component = GetComponent<T>())
		{
			return component;
		}

		auto newComponent = dynamic_cast<T*>(m_Components.emplace_back(static_cast<Component*>(new T(this))));
		assert(newComponent);

		// awake가 있다면 호출

		EVENT_FIRE(eEventType::SceneResolve);

		return newComponent;
	}

	template<typename T>
	void GameObject::RemoveComponent()
	{
		auto& it = m_Components.begin();
		for (it; it != m_Components.end();)
		{
			if((*it)->GetType() == Component::TypeToEnum<T>())
			{
				delete (*it);
				(*it) = nullptr;
				(*it).reset();
				it = m_Components.erase(it);
				return;
			}
			else
				it++;
		}

		// 이건 추후 확인 필요
		EVENT_FIRE(eEventType::SceneResolve);
	}

	template<typename T>
	T* GameObject::GetComponent()
	{
		for (auto component : m_Components)
		{
			if (component->GetType() == Component::TypeToEnum<T>())
			{
				return dynamic_cast<T*>(component);
			}
		}

		return nullptr;
	}
}