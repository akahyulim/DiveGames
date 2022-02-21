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

	// 계층 구조를 형성했을 경우 자식까지 모두 제거한다.
	void Scene::RemoveGameObject(unsigned long long id)
	{
		// 바로 제거하면 안될듯?
	}

	void Scene::RemoveGameObject(GameObject* pTarget)
	{
		if (!pTarget)
			return;

		RemoveGameObject(pTarget->GetInstanceID());
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