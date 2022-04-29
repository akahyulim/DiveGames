#pragma once
#include "Base/Object.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Component/Camera.h"
#include "Component/SpriteRenderable.h"
#include "Component/MeshRenderable.h"
#include "Events/EngineEvents.h"

namespace Dive
{
	class Scene;

	class GameObject : public Object
	{
	public:
		GameObject(Scene* pScene, const std::string& name = std::string(), unsigned long long id = 0);
		~GameObject();

		template<class T>
		T* AddComponent(unsigned long long id = 0)
		{
			if (HasComponent<T>())
				return GetComponent<T>();

			T* pAddedComponent = new T(this, id);
			m_Components.emplace_back(static_cast<Component*>(pAddedComponent));
		
			FIRE_EVENT(GameObjectModifyEvent(static_cast<Component*>(pAddedComponent)));

			return pAddedComponent;
		}

		Component* AddComponent(eComponentType type, unsigned long long id = 0)
		{
			if (HasComponent(type))
				return GetComponent(type);

			Component* pAddedComponent = nullptr;

			switch (type)
			{
			case eComponentType::Transform:
				pAddedComponent = new Transform(this, id);
				break;
			case eComponentType::Camera:
				//pAddedComponent = new Camera(this, id);
				break;
			case eComponentType::SpriteRenderable:
				pAddedComponent = new SpriteRenderable(this, id);
				break;
			case eComponentType::MeshRenderable:
				pAddedComponent = new MeshRenderable(this, id);
				break;

			default:
				return nullptr;
			}

			m_Components.emplace_back(pAddedComponent);

			FIRE_EVENT(GameObjectModifyEvent(pAddedComponent));

			return pAddedComponent;
		}
		
		template<class T>
		void RemoveComponent()
		{
			// ���� �̷��� �� �ʿ䰡 ���� �� �ϴ�.
			auto it = m_Components.begin();
			for (it; it != m_Components.end();)
			{
				if (it->GetType() == T::GetStaticType())
				{
					FIRE_EVENT(GameObjectModifyEvent(static_cast<Component*>(*it)));

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
			for (auto pComponent : m_Components)
			{
				if (pComponent->GetType() == type)
					return true;
			}

			return false;
		}

		template<class T>
		bool HasComponent()
		{
			return HasComponent(T::GetStaticType());
		}

		// ��� ������ ��� �θ� Ÿ�����κ��� ���� �� �ֵ��� �ϰ� �ʹ�...
		template<class T>
		T* GetComponent()
		{
			for (auto pComponent : m_Components)
			{
				if (pComponent->GetType() == T::GetStaticType())
				{
					return dynamic_cast<T*>(pComponent);
				}
			}

			return nullptr;
		}

		Component* GetComponent(eComponentType type)
		{
			for (auto pComponent : m_Components)
			{
				if (pComponent->GetType() == type)
					return static_cast<Component*>(pComponent);
			}

			return nullptr;
		}

		std::vector<Component*> GetComponents()
		{
			// ���� ������ ���̴�.
			return m_Components;
		}

		bool IsActive() const { return m_bActive; }
		void SetActive(bool active) { m_bActive = active; }

		bool IsRemoveTarget() const { return m_bRemoveTarget; }
		void MarkRemoveTarget() { m_bRemoveTarget = true; }

	private:
	private:
		Scene* m_pScene			= nullptr;
		std::vector<Component*> m_Components;

		bool m_bActive			= true;
		bool m_bRemoveTarget	= false;
	};
}