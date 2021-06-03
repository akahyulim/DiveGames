#include "Scene.h"
#include "GameObject.h"
#include "Log.h"
#include "Transform.h"

namespace Dive
{
	Scene::Scene()
		: Object(typeid(Scene).hash_code())
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::Update(float deltaTime)
	{
		CORE_TRACE("Scene::Update() - deltaTime * timeScale이다.");
		CORE_TRACE("Scene::Update() - 대상은 모든 GameObjects다.");

		for (auto& gameObject : m_gameObjects)
		{
			gameObject->Update(deltaTime);
		}
	}

	void Scene::Clear()
	{
		m_gameObjects.clear();
	}

	void Scene::LoadFromFile(const std::string& filepath)
	{
	}

	void Scene::SaveToFile(const std::string& filepath)
	{
	}

	GameObject* Scene::CreateGameObject()
	{
		auto newGameObject = m_gameObjects.emplace_back(std::make_shared<GameObject>());
		// 뭔가 호출?

		return newGameObject.get();
	}

	GameObject* Scene::GetGameObjectByName(const std::string& name)
	{
		for (auto& target : m_gameObjects)
		{
			if (target->GetName() == name)
				return target.get();
		}

		return nullptr;
	}

	GameObject* Scene::GetGameObjectByID(unsigned int id)
	{
		for (auto& target : m_gameObjects)
		{
			if (target->GetID() == id)
				return target.get();
		}

		return nullptr;
	}

	void Scene::RemoveGameObject(GameObject* target)
	{
		assert(target != nullptr);

		// 스파르탄은 바로 지우지 않는다. 일단 제거 대상으로 설정해놓는다.
		// 그리고 다음 Frame에서 제거한다.

		gameObjectRemove(target);
	}

	std::vector<GameObject*> Scene::GetRootGameObjects()
	{
		std::vector<GameObject*> rootGameObjects;
		for (auto pGameObject : m_gameObjects)
		{
			auto pTransform = pGameObject->GetComponent<Transform>();
			if (pTransform)
			{
				if (!pTransform->HasParent())
				{
					rootGameObjects.emplace_back(pGameObject.get());
				}
			}
		}

		return rootGameObjects;
	}

	void Scene::gameObjectRemove(GameObject* pGameObject)
	{
		// 자식들부터 먼저 제거
		auto pChildren = pGameObject->GetTransform()->GetChildren();
		for (auto pChild : pChildren)
		{
			gameObjectRemove(pChild->GetOwner());
		}

		auto pParent = pGameObject->GetTransform()->GetParent();

		// 부모에게서 제외되는 구문이 없다.
		for (auto it = m_gameObjects.begin(); it != m_gameObjects.end();)
		{
			auto pTarget = *it;
			if (pTarget->GetID() == pGameObject->GetID())
			{
				it = m_gameObjects.erase(it);
				break;
			}
			++it;
		}

		if (pParent)
		{
			pParent->AcquireChidren();
		}
	}
}