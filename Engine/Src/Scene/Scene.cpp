#include "divepch.h"
#include "Scene.h"
#include "GameObject.h"
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
		auto pNewGameObject = m_GameObjects.emplace_back(new GameObject(this));
		DV_ASSERT(pNewGameObject != nullptr);
		pNewGameObject->SetName(name);
		pNewGameObject->AddComponent<Transform>();

		return pNewGameObject;
	}

	// 계층 구조를 형성했을 경우 자식까지 모두 제거한다.
	void Scene::RemoveGameObject(GameObject* pTarget)
	{
		if (!pTarget)
			return;

		// 바로 지우면 안될 것 같다.
	}
}