#include "divepch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Core/EventSystem.h"
#include "Engine/EngineEvents.h"
#include "IO/Log.h"


namespace Dive
{
	Scene::Scene(Context* pContext)
		: Object(pContext),
		m_Name("Untitled"),
		m_bUpdateEnabled(true),
		m_bDirty(false),
		m_CurGameObjectID(FIRST_ID),
		m_CurComponentID(FIRST_ID),
		m_OnUpdateSlotID(0)
	{
		m_OnUpdateSlotID = SUBSCRIBE_EVENT(eEventType::Update, EVENT_HANDLER_PARAM(OnUpdate));
	}

	Scene::~Scene()
	{
		Clear();

		// Sceneó�� �� �̻��� ��ü�� ������ �� ���������� ��������� ���־�� �Ѵ�.
		// �׷��� ������ Fire���� �̹� ������ ��󿡰� �̺�Ʈ�� �����ϴ� �������� ������ �߻��Ѵ�.
		UNSUBSCRIBE_EVENT(eEventType::Update, m_OnUpdateSlotID);

		DV_LOG_ENGINE_TRACE("Scene �Ҹ� �Ϸ�({:s})", m_Name);
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

		m_CurGameObjectID = FIRST_ID;
		m_CurComponentID = FIRST_ID;
	}

	void Scene::Update(float delta)
	{
		// Start, Stop�� �������κ��� ���¸� Ȯ���ؾ� �Ѵ�.
		for (auto pGameObject : m_GameObjects)
		{
			pGameObject.second->Update(delta);
		}

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

		// ���⿡���� �̺�Ʈ�� ������.
		// �����ʹ� �ڽ��� �����Ϳ�, delta�� timeScale�� ���� ��
	}

	GameObject* Scene::CreateGameObject(const std::string& name, uint32_t id)
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

		auto* pTransformCom = static_cast<Component*>(pNewGameObject->GetComponent<Transform>());
		RegisterComponent(pTransformCom, pTransformCom->GetID());

		return pNewGameObject;
	}

	void Scene::RemoveGameObject(GameObject* pGameObject)
	{
		if (!pGameObject || m_GameObjects.empty())
			return;

		RemoveGameObject(pGameObject->GetID());
	}

	void Scene::RemoveGameObject(uint32_t id)
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

	GameObject* Scene::GetGameObject(uint32_t id)
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

	std::vector<GameObject*> Scene::GetAllGameObjects() const
	{
		if(m_GameObjects.empty())
			return std::vector<GameObject*>();

		std::vector<GameObject*> ret;
		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end(); ++it)
			ret.emplace_back(it->second);

		return ret;
	}

	void Scene::RegisterComponent(Component* pComponent, uint32_t id)
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

	void Scene::DeregisterComponent(Component* pComponent)
	{
		if (!pComponent)
			return;

		auto comID = pComponent->GetID();
		if (m_Components.find(comID) != m_Components.end())
		{
			m_Components.erase(comID);
		}
	}

	Component* Scene::GetComponent(uint32_t id)
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
		Update(evnt.GetTimeStep());
	}

	// ����⿡ �� ���� �������� �� �� ����...
	uint32_t Scene::getFreeGameObjectID()
	{
		for (;;)
		{
			auto freeID = m_CurGameObjectID;

			if (m_CurGameObjectID < LAST_ID)
				++m_CurGameObjectID;
			else
				m_CurGameObjectID = FIRST_ID;
		
			if (!m_GameObjects[freeID])
				return freeID;
		}
	}

	uint32_t Scene::getFreeComponentID()
	{
		for (;;)
		{
			auto freeID = m_CurComponentID;

			if (m_CurComponentID < LAST_ID)
				++m_CurComponentID;
			else
				m_CurComponentID = FIRST_ID;

			if (!m_Components[freeID])
				return freeID;
		}
	}
}