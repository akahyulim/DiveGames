#pragma once
#include "Core/Object.h"
#include "Scene/Scene.h"

namespace Dive
{
	class Component;
	class Transform;
	class Drawable;

	class GameObject : public Object
	{
	public:
		GameObject(const std::string& name = "GameObject");
		~GameObject() override;

		// start, stop 그리고 preUpdate 추가?
		void Update();

		bool LoadFromFile(const std::string& name);
		bool SaveToFile(const std::string& name);

		template<class T>
		T* AddComponent(uint64_t id = 0)
		{
			auto pExisted = GetComponent<T>();
			if (pExisted)
				return pExisted;

			auto pNewComponent = new T(this);
			m_Components.emplace_back(pNewComponent);
			Scene::RegisterComponent(pNewComponent, id);

			return pNewComponent;
		}

		template<class T>
		bool HasComponent()
		{
			for (auto pComponent : m_Components)
			{
				if (dynamic_cast<T*>(pComponent))
					return true;
			}
			return false;
		}

		template<class T>
		T* GetComponent()
		{
			for (auto pComponent : m_Components)
			{
				T* pTargetComponent = dynamic_cast<T*>(pComponent);
				if (pTargetComponent != nullptr)
					return pTargetComponent;
			}

			return nullptr;
		}

		bool IsActive() const { return m_bActive; }
		void SetActive(bool active) { m_bActive = active; }

		void MarkRemoveTarget() { m_bMarkedTarget = true; }
		bool IsRemovedTarget() const { return m_bMarkedTarget; }

		Transform* GetTransform() const { return m_pTransform; }

	private:
	private:
		bool m_bActive;
		bool m_bMarkedTarget;
		std::vector<Component*> m_Components;

		Transform* m_pTransform;
	};
}
