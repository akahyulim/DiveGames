#include "divepch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Core/EventSystem.h"
#include "Core/CoreEvents.h"
#include "IO/Log.h"


namespace Dive
{
	Scene::Scene(Context* pContext)
		: Object(pContext),
		m_bUpdateEnabled(true),
		m_GameObjectID(FIRST_ID)
	{
		SUBSCRIBE_EVENT(eEventType::Update, EVENT_HANDLER(OnUpdate));
	}

	Scene::~Scene()
	{
		Clear();

		DV_LOG_ENGINE_DEBUG("Scene �Ҹ��� ȣ��");
	}

	void Scene::Clear()
	{
		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end(); ++it)
		{
			DV_DELETE(it->second);
		}
		m_GameObjects.clear();

		m_GameObjectID = FIRST_ID;
	}

	void Scene::Update(float delta)
	{
		// ���⿡���� �̺�Ʈ�� ������.
		// �����ʹ� �ڽ��� �����Ϳ�, delta�� timeScale�� ���� ��
	}

	GameObject* Scene::CreateGameObject(const std::string& name, unsigned int id)
	{
		auto* pNewGameObject = new GameObject(m_pContext);
		pNewGameObject->SetName(name.empty() ? "GameObject" : name);

		if (!id || GetGameObject(id))
		{
			id = getFreeID();
		}
		pNewGameObject->SetID(id);

		m_GameObjects[id] = pNewGameObject;

		return pNewGameObject;
	}

	void Scene::RemoveGameObject(GameObject* pGameObject)
	{
		if (!pGameObject || m_GameObjects.empty())
			return;

		RemoveGameObject(pGameObject->GetID());
	}

	// �Ƹ� �̴�� ����ϸ� �ȵɰŴ�.
	// ���� �������� ���� �� �����ؾ� �Ѵ�.
	void Scene::RemoveGameObject(unsigned int id)
	{
		if (!id || m_GameObjects.empty())
			return;

		auto it = m_GameObjects.find(id);
		if (it != m_GameObjects.end())
		{
			DV_DELETE(it->second);
		}
	}

	GameObject* Scene::GetGameObject(unsigned int id)
	{
		auto it = m_GameObjects.find(id);

		return it != m_GameObjects.end() ? it->second : nullptr;
	}

	void Scene::SetUpdateEnabled(bool bEnable)
	{
		m_bUpdateEnabled = bEnable;
	}

	void Scene::OnUpdate(const Event& e)
	{
		if (!m_bUpdateEnabled)
			return;

		auto evnt = dynamic_cast<const UpdateEvent&>(e);
		Update(evnt.GetDeltaTime());
	}

	// ����⿡ �� ���� �������� �� �� ����...
	unsigned int Scene::getFreeID()
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
}