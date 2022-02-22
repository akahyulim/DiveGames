#include "divepch.h"
#include "Scene.h"
#include "Base/Base.h"
#include "Component/Transform.h"

namespace Dive
{
	Scene::Scene(const std::string& name)
		: m_Name(name)
	{
		if (m_Name.empty())
			m_Name = "Untitled";
	}

	void Scene::UpdateRuntime(float elapsedTime)
	{
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

	// �ϴ� ���������� �ڽŸ� �����Ѵ�.
	void Scene::RemoveGameObject(unsigned long long id)
	{
		// ��Ÿ�ӿ��� ������ �� �� �ִ�?
		auto it = m_GameObjects.begin();
		for (it; it != m_GameObjects.end();)
		{
			if ((*it)->GetInstanceID() == id)
			{
				// �θ� ������ �� ������ �ȴ�.
				DV_DELETE((*it));
				m_GameObjects.erase(it);
				break;
			}
			else
			{
				it++;
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