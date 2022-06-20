#pragma once
#include "Base/Object.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Component/Camera.h"
#include "Component/Light.h"
#include "Component/SpriteRenderable.h"
#include "Component/MeshRenderable.h"
#include "Events/EngineEvents.h"

namespace Dive
{
	class Scene;

	enum class ePrimitiveType
	{
		Sphere,
		Capsule,
		Cylinder,
		Cube,
		Plane,
		Quad
	};

	class GameObject : public Object
	{
	public:
		GameObject(Scene* pScene);
		~GameObject();

		template<class T>
		T* AddComponent()
		{
			if (HasComponent<T>())
				return GetComponent<T>();

			auto newCom = dynamic_cast<T*>(m_Components.emplace_back(new T(this)));

			FIRE_EVENT(ModifyGameObjectEvent(static_cast<Component*>(newCom)));

			return newCom;
		}

		Component* AddComponent(eComponentType type)
		{
			if (HasComponent(type))
				return GetComponent(type);

			Component* newCom = nullptr;

			switch (type)
			{
			case eComponentType::Transform:
				newCom = new Transform(this);
				break;
			case eComponentType::Camera:
				//newCom = new Camera(this);
				break;
			case eComponentType::SpriteRenderable:
				newCom = new SpriteRenderable(this);
				break;
			case eComponentType::MeshRenderable:
				newCom = new MeshRenderable(this);
				break;

			case eComponentType::Light:
				newCom = new Light(this);
				break;

			default:
				return nullptr;
			}

			m_Components.emplace_back(newCom);

			FIRE_EVENT(ModifyGameObjectEvent(newCom));

			return newCom;
		}
		
		template<class T>
		void RemoveComponent()
		{
			// 굳이 이렇게 할 필요가 없을 듯 하다. => 무슨 의미?
			auto it = m_Components.begin();
			for (it; it != m_Components.end();)
			{
				if (it->GetType() == T::GetStaticType())
				{
					FIRE_EVENT(ModifyGameObjectEvent(static_cast<Component*>(*it)));

					DV_DELETE(*it);
					m_Components.erase(it);

					return;
				}
				else
				{
					it++;
				}
			}
		}

		bool HasComponent(eComponentType type)
		{
			for (auto com : m_Components)
			{
				if (com->GetType() == type)
					return true;
			}

			return false;
		}

		template<class T>
		bool HasComponent()
		{
			return HasComponent(T::GetStaticType());
		}

		// 상속 구현일 경우 부모 타입으로부터 얻을 수 있도록 하고 싶다...
		template<class T>
		T* GetComponent()
		{
			for (auto com : m_Components)
			{
				if (com->GetType() == T::GetStaticType())
				{
					return dynamic_cast<T*>(com);
				}
			}

			return nullptr;
		}

		Component* GetComponent(eComponentType type)
		{
			for (auto com : m_Components)
			{
				if (com->GetType() == type)
					return static_cast<Component*>(com);
			}

			return nullptr;
		}

		std::vector<Component*> GetComponents()
		{
			// 복사 생성일 것이다.
			return m_Components;
		}

		bool IsActive() const { return m_bActive; }
		void SetActive(bool active) { m_bActive = active; }

		bool IsRemoveTarget() const { return m_bRemoveTarget; }
		void MarkRemoveTarget() { m_bRemoveTarget = true; }

		Scene* GetScene() { return m_pScene; }
		void SetScene(Scene* pScene) { m_pScene = pScene; }

		GameObject* Clone();

		static GameObject* CreatePrimitive(Scene* pScene, ePrimitiveType type);


	private:
	private:
		Scene* m_pScene			= nullptr;
		std::vector<Component*> m_Components;

		bool m_bActive			= true;
		bool m_bRemoveTarget	= false;
	};
}