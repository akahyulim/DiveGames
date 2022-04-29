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

		SUBSCRIBE_EVENT(eEventType::GameObjectModify, EVENT_HANDLER(OnGameObjectModify));
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
					// 이렇게 하나만 제거하면 안될 것 같다.
					DV_DELETE(*it);
					it = m_GameObjects.erase(it);
				}
				else
					it++;
			}

			FIRE_EVENT(SceneResolveEvent(&m_GameObjects));

			m_bDirty = false;
		}

		// 현재 GameObject와 Component에는 Update가 없다.
		// 유니티처럼 한다면 아래의 update script에서 처리하는 모양이 나올 듯?

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

	GameObject* Scene::CreateGameObject(const std::string& name, unsigned long long id)
	{
		return m_GameObjects.emplace_back(new GameObject(this, name, id));
	}

	// 자식까지 모두 제거
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
			if( pTransform )
			{
				if (!pTransform->HasParent())
				{
					roots.emplace_back(pGameObject);
				}
			}
			else
			{
				roots.emplace_back(pGameObject);
			}
		}

		return roots;
	}

	std::vector<Component*> Scene::GetComponents(eComponentType type)
	{
		std::vector<Component*> components;
		if (!m_Components[type].empty())
			components = m_Components[type];

		return components;
	}

	Component* Scene::GetComponent(eComponentType type, unsigned long long id)
	{
		auto components = GetComponents(type);
		if (!components.empty())
		{
			for (auto pComponent : components)
			{
				if (pComponent->GetInstanceID() == id)
					return pComponent;
			}
		}

		return nullptr;
	}

	void Scene::OnGameObjectModify(const Event& e)
	{
		m_bDirty = true;

		auto pComponent = dynamic_cast<const GameObjectModifyEvent&>(e).GetComponent();
		auto type = pComponent->GetType();

		if (!m_Components[type].empty())
		{
			auto it = m_Components[type].begin();
			for (it; it != m_Components[type].end(); it++)
			{
				if ((*it)->GetInstanceID() == pComponent->GetInstanceID())
				{
					m_Components[type].erase(it);
					return;
				}
			}
		}

		m_Components[type].push_back(const_cast<Component*>(pComponent));
	}

}