#pragma once
#include "Base/Object.h"
#include "Component/Transform.h"
#include "Component/Camera.h"

namespace Dive
{
	class Scene;

	class GameObject : public Object
	{
	public:
		GameObject(Scene* pScene, const std::string& name = std::string());
		GameObject(Scene* pScene, unsigned long long id, const std::string& name = std::string());
		~GameObject() = default;

		template<class T>
		T* AddComponent()
		{
			if (HasComponent<T>())
				return GetComponent<T>();

			auto pNewComponent = m_Components.emplace_back(static_cast<Component*>(new T(this)));
			DV_ASSERT(pNewComponent);

			return dynamic_cast<T*>(pNewComponent);
		}
		
		template<class T>
		void RemoveComponent()
		{
			auto it = m_Components.begin();
			for (it; it != m_Components.end();)
			{
				if (it->GetType() == Component::TypeToEnum<T>())
				{
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

		template<class T>
		bool HasComponent()
		{
			for (auto pComponent : m_Components)
			{
				if (pComponent->GetType() == Component::TypeToEnum<T>())
					return true;
			}

			return false;
		}

		template<class T>
		T* GetComponent()
		{
			for (auto pComponent : m_Components)
			{
				if (pComponent->GetType() == Component::TypeToEnum<T>())
				{
					return dynamic_cast<T*>(pComponent);
				}
			}

			return nullptr;
		}

	private:
	private:
		Scene* m_pScene			= nullptr;
		std::vector<Component*> m_Components;
	};
}