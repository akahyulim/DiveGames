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
		GameObject(Scene* pScene, const std::string& name = std::string());
		GameObject(Scene* pScene, unsigned long long id, const std::string& name = std::string());
		~GameObject();

		template<class T>
		T* AddComponent()
		{
			if (HasComponent<T>())
				return GetComponent<T>();

			T* pNewComponent = new T(this);
			m_Components.emplace_back(static_cast<Component*>(pNewComponent));
		
			FIRE_EVENT(GameObjectModifyEvent());

			return pNewComponent;
		}
		
		template<class T>
		void RemoveComponent()
		{
			auto it = m_Components.begin();
			for (it; it != m_Components.end();)
			{
				if (it->GetType() == T::GetStaticType())
				{
					DV_DELETE(*it);
					m_Components.erase(it);

					FIRE_EVENT(GameObjectModifyEvent());
					
					return;
				}
				else
				{
					it++;
				}
			}
		}

		template<class T>
		bool HasComponent()
		{
			for (auto pComponent : m_Components)
			{
				if (pComponent->GetType() == T::GetStaticType())
					return true;
			}

			return false;
		}

		// 상속 구현일 경우 부모 타입으로부터 얻을 수 있도록 하고 싶다...
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