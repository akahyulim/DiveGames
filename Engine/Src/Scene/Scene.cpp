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

	// ���� ������ �������� ��� �ڽı��� ��� �����Ѵ�.
	void Scene::RemoveGameObject(GameObject* pTarget)
	{
		if (!pTarget)
			return;

		// �ٷ� ����� �ȵ� �� ����.
	}
}