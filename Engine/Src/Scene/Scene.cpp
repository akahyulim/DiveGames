#include "divepch.h"
#include "Scene.h"
#include "Base/Base.h"
#include "Component/Transform.h"
#include "Events/EngineEvents.h"

namespace Dive
{
	Scene::Scene(const std::string& name)
		: m_Name(name)
	{
		if (m_Name.empty())
			m_Name = "Untitled";

		SUBSCRIBE_EVENT(eEventType::GameObjectModify, [this](const Event& e) { m_bDirty = true; });
	}

	Scene::~Scene()
	{
		Clear();
	}

	void Scene::Update(float elapsedTime)
	{
		if (m_bDirty)
		{
			auto it = m_GameObjects.begin();
			for (it; it != m_GameObjects.end();)
			{
				if ((*it)->IsRemoveTarget())
				{
					// �̷��� �ϳ��� �����ϸ� �ȵ� �� ����.
					DV_DELETE(*it);
					it = m_GameObjects.erase(it);
				}
				else
					it++;
			}

			FIRE_EVENT(SceneResolveEvent(&m_GameObjects));

			m_bDirty = false;
		}

		// ���� GameObject�� Component���� Update�� ����.
		// ����Ƽó�� �Ѵٸ� �Ʒ��� update script���� ó���ϴ� ����� ���� ��?

		// update script

		// set camera

		// begin / end scene
	}

	void Scene::Clear()
	{
		if (!m_GameObjects.empty())
		{
			for (auto pGameObject : m_GameObjects)
			{
				DV_DELETE(pGameObject);
			}
			m_GameObjects.clear();
			m_GameObjects.shrink_to_fit();
		}
	}

	GameObject* Scene::CreateGameObject(const std::string& name)
	{
		auto pNewGameObject = m_GameObjects.emplace_back(new GameObject(this, name));
		DV_ASSERT(pNewGameObject != nullptr);
		pNewGameObject->AddComponent<Transform>();

		return pNewGameObject;
	}

	GameObject* Scene::CreateGameObject(unsigned long long id, const std::string& name)
	{
		auto pNewGameObject = m_GameObjects.emplace_back(new GameObject(this, id, name));
		DV_ASSERT(pNewGameObject != nullptr);
		pNewGameObject->AddComponent<Transform>();

		return pNewGameObject;
	}

	// �ڽı��� ��� ����
	void Scene::RemoveGameObject(unsigned long long id)
	{
		if (m_GameObjects.empty())
			return;

		if (!m_bDirty)
			m_bDirty = true;

		for (auto pGameObject : m_GameObjects)
		{
			if (pGameObject->GetInstanceID() == id)
			{
				auto pTransform = pGameObject->GetComponent<Transform>();
				if (pTransform)
				{
					if (pTransform->HasChildren())
					{
						for (auto pChild : pTransform->GetChildren())
						{
							RemoveGameObject(pChild->GetGameObject()->GetInstanceID());
						}
					}

					if (pTransform->HasParent())
					{
						pTransform->SetParent(nullptr);
					}
				}

				pGameObject->MarkRemoveTarget();
			}
		}
	}

	void Scene::RemoveGameObject(GameObject* pTarget)
	{
		if (!pTarget)
			return;

		RemoveGameObject(pTarget->GetInstanceID());
	}

	GameObject* Scene::GetGameObject(unsigned long long id)
	{
		for (auto pGameObject : m_GameObjects)
		{
			if (pGameObject->GetInstanceID() == id)
				return pGameObject;
		}

		return nullptr;
	}

	std::vector<GameObject*> Scene::GetRoots()
	{
		std::vector<GameObject*> roots;

		for (auto& pGameObject : m_GameObjects)
		{
			auto pTransform = pGameObject->GetComponent<Transform>();
			if (!pTransform)
				continue;

			if (!pTransform->HasParent())
				roots.emplace_back(pGameObject);
		}

		return roots;
	}
}