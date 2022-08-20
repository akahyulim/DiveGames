#include "divepch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Core/EventSystem.h"
#include "Core/CoreEvents.h"
#include "IO/Log.h"


namespace Dive
{
	Scene::Scene(Context* pContext)
		: Object(pContext),
		m_Name("Untitled"),
		m_bUpdateEnabled(true),
		m_bDirty(false),
		m_GameObjectID(FIRST_ID),
		m_ComponentID(FIRST_ID),
		m_OnUpdateSlotID(0)
	{
		m_OnUpdateSlotID = SUBSCRIBE_EVENT(eEventType::Update, EVENT_HANDLER_PARAM(OnUpdate));
	}

	Scene::~Scene()
	{
		Clear();

		UNSUBSCRIBE_EVENT(eEventType::Update, m_OnUpdateSlotID);

		DV_LOG_ENGINE_DEBUG("Scene({:s}) 소멸자 호출", m_Name);
	}

	void Scene::Clear()
	{
		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end(); ++it)
		{
			DV_DELETE(it->second);
		}
		m_GameObjects.clear();
		m_Components.clear();

		m_GameObjectID = FIRST_ID;
		m_ComponentID = FIRST_ID;
	}

	void Scene::Update(float delta)
	{
		if (m_bDirty)
		{
			auto it = m_GameObjects.begin();
			for (it; it != m_GameObjects.end();)
			{
				if (it->second->IsRemovedTarget())
				{
					DV_DELETE(it->second);
					it = m_GameObjects.erase(it);
				}
				else
					it++;
			}

			m_bDirty = false;
		}

		// 여기에서도 이벤트를 날린다.
		// 데이터는 자신의 포인터와, delta에 timeScale을 곱한 값
	}

	GameObject* Scene::CreateGameObject(const std::string& name, unsigned int id)
	{
		auto* pNewGameObject = new GameObject(m_pContext);
		pNewGameObject->SetName(name.empty() ? "GameObject" : name);
		pNewGameObject->setScene(this);

		if (!id || GetGameObject(id))
		{
			id = getFreeGameObjectID();
		}
		pNewGameObject->setID(id);

		m_GameObjects[id] = pNewGameObject;

		return pNewGameObject;
	}

	void Scene::RemoveGameObject(GameObject* pGameObject)
	{
		if (!pGameObject || m_GameObjects.empty())
			return;

		RemoveGameObject(pGameObject->GetID());
	}

	void Scene::RemoveGameObject(unsigned int id)
	{
		if (!id || m_GameObjects.empty())
			return;

		if(!m_bDirty)
			m_bDirty = true;

		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end(); ++it)
		{
			if (it->first == id)
			{
				auto* pTransform = it->second->GetComponent<Transform>();
				if (pTransform)
				{
					if (pTransform->HasChild())
					{
						for (auto* pChild : pTransform->GetChildren())
						{
							RemoveGameObject(pChild->GetGameObject()->GetID());
						}
					}

					if (pTransform->HasParent())
						pTransform->SetParent(nullptr);
				}

				it->second->MarkRemoveTarget();
			}
		}
	}

	GameObject* Scene::GetGameObject(unsigned int id)
	{
		auto it = m_GameObjects.find(id);
		return it != m_GameObjects.end() ? it->second : nullptr;
	}

	std::vector<GameObject*> Scene::GetRoots() const
	{
		std::vector<GameObject*> roots;

		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end(); ++it)
		{
			auto pTransform = it->second->GetComponent<Transform>();
			if (pTransform)
			{
				if (!pTransform->HasParent())
					roots.emplace_back(it->second);
			}
		}

		return roots;
	}

	void Scene::ComponentAdded(Component* pComponent, unsigned int id)
	{
		if (!pComponent)
			return;

		if (!id || GetComponent(id))
		{
			id = getFreeComponentID();
		}
		pComponent->setID(id);

		m_Components[id] = pComponent;
	}

	void Scene::ComponentRemoved(Component* pComponent)
	{
		if (!pComponent)
			return;

		auto comID = pComponent->GetID();
		if (GetComponent(comID))
		{
			m_Components.erase(comID);
		}
	}

	Component* Scene::GetComponent(unsigned int id)
	{
		auto it = m_Components.find(id);
		return it != m_Components.end() ? it->second : nullptr;
	}

	void Scene::SetUpdateEnabled(bool bEnable)
	{
		m_bUpdateEnabled = bEnable;
	}

	void Scene::OnUpdate(const Event& e)
	{
		if (!m_bUpdateEnabled)
			return;

		auto& evnt = dynamic_cast<const UpdateEvent&>(e);
		Update(evnt.GetDeltaTime());
	}

	// 언듯보기에 꽉 차면 무한으로 돌 거 같다...
	unsigned int Scene::getFreeGameObjectID()
	{
		for (;;)
		{
			auto freeID = m_GameObjectID;

			if (m_GameObjectID < LAST_ID)
				++m_GameObjectID;
			else
				m_GameObjectID = FIRST_ID;
		
			if (!m_GameObjects[freeID])
				return freeID;
		}
	}

	// 빈 공간을 찾는 다는건 한 공간에 관리 중이라는 말이다.
	// 즉, 컨테이너를 scene에 마련해야 한다.
	unsigned int Scene::getFreeComponentID()
	{
		for (;;)
		{
			auto freeID = m_ComponentID;

			if (m_ComponentID < LAST_ID)
				++m_ComponentID;
			else
				m_ComponentID = FIRST_ID;

			if (!m_Components[freeID])
				return freeID;
		}
	}
}