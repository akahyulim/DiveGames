#pragma once
#include "Core/Object.h"
#include "DvScene.h"

namespace Dive
{
	class DvScene;
	class DvComponent;
	class DvTransform;

	class DvGameObject : public Object
	{
	public:
		DvGameObject(DvScene* pScene, const std::string& name = "DvGameObject");
		~DvGameObject() override;

		void Update();

		template<class T>
		T* AddComponent(uint64_t id = 0)
		{
			auto pExisted = GetComponent<T>();
			if (pExisted)
				return pExisted;

			auto pNewComponent = new T(this);
			m_Components.emplace_back(pNewComponent);
			m_pScene->RegisterComponent(pNewComponent, id);

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

		DvTransform* GetTransform() const { return m_pTransform; }

	private:
		DvScene* m_pScene;
		bool m_bActive;
		bool m_bMarkedTarget;
		std::vector<DvComponent*> m_Components;

		DvTransform* m_pTransform;
	};
}