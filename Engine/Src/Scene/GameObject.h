#pragma once
#include "Core/Object.h"
#include "Core/StringHash.h"
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

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		Scene* GetScene() const { return m_pScene; }
		unsigned int GetID() const { return m_ID; }

		template<class T>
		T* CreateComponent(unsigned int id = 0);

		template<class T>
		void RemoveComponent();
		void RemoveComponent(Component* pComponent);

		template<class T>
		T* GetComponent() const;

	private:
		void setScene(Scene* pScene) { m_pScene = pScene; }
		void setID(unsigned int id) { m_ID = id; }

	private:
		Scene* m_pScene;
		std::string m_Name;
		unsigned int m_ID;

		std::vector<Component*> m_Components;
	};

	template<class T>
	T* GameObject::CreateComponent(unsigned int id)
	{
		if (GetComponent<T>())
		{
			DV_LOG_ENGINE_WRAN("이미 존재하는 타입의 컴포넌트 생성을 시도하였습니다.");
			return nullptr;
		}

		auto* pNewComponent = new T(m_pContext);
		pNewComponent->setGameObject(this);

		m_Components.emplace_back(pNewComponent);
		if (m_pScene)
			m_pScene->ComponentAdded(pNewComponent, id);
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
			if (pComponent->GetType() == T::GetTypeStatic())
				return dynamic_cast<T*>(pComponent);
		}

		return nullptr;
	}
}