#pragma once
#include "Core/Object.h"
#include "Math/StringHash.h"
#include "Scene.h"
#include "IO/Log.h"

namespace Dive
{
	class Context;
	class Component;

	class GameObject : public Object
	{
		DIVE_OBJECT(GameObject, Object)

		friend class Scene;

	public:
		explicit GameObject(Context* pContext);
		~GameObject();

		void Start();
		void Stop();
		void Update(float delta);

		template<class T> T* AddComponent(uint32_t id = 0);

		template<class T> void RemoveComponent();
		void RemoveComponent(Component* pComponent);

		template<class T> T* GetComponent() const;

		template<class T> bool HasComponent() const;

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		Scene* GetScene() const { return m_pScene; }
		uint32_t GetID() const { return m_ID; }

		bool IsActive() const { return m_bActive; }
		void SetActive(bool bActive) { m_bActive = bActive; }

		void MarkRemoveTarget() { m_bMarkedTarget = true; }
		bool IsRemovedTarget() const { return m_bMarkedTarget; }

	private:
		void setScene(Scene* pScene) { m_pScene = pScene; }
		void setID(uint32_t id) { m_ID = id; }

	private:
		Scene* m_pScene;
		std::string m_Name;
		uint32_t m_ID;
		bool m_bActive;
		bool m_bMarkedTarget;

		std::vector<Component*> m_Components;
	};

	template<class T>
	T* GameObject::AddComponent(uint32_t id)
	{
		if (GetComponent<T>())
		{
			DV_LOG_ENGINE_WARN("GameObject::AddComponent - 이미 존재하는 타입의 컴포넌트 생성을 시도하였습니다.");
			return nullptr;
		}

		auto* pNewComponent = new T(m_pContext);
		pNewComponent->setGameObject(this);

		m_Components.emplace_back(pNewComponent);
		if (m_pScene)
			m_pScene->RegisterComponent(pNewComponent, id);
		else
			pNewComponent->setID(id);

		return pNewComponent;
	}

	template<class T>
	void GameObject::RemoveComponent()
	{
		RemoveComponent(static_cast<Component*>(GetComponent<T>()));
	}

	template<class T>
	T* GameObject::GetComponent() const
	{
		for (auto* pComponent : m_Components)
		{
			if (pComponent->GetTypeHash() == T::GetTypeStatic())
				return dynamic_cast<T*>(pComponent);
		}

		return nullptr;
	}

	template<class T>
	bool GameObject::HasComponent() const
	{
		for (auto* pComponent : m_Components)
		{
			if (pComponent->GetTypeHash() == T::GetTypeStatic())
				return true;
		}

		return false;
	}
}