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
	
	void Scene::RemoveGameObjectByName(const std::string& name)
	{
		auto it = m_gameObjects.begin();
		for (it; it != m_gameObjects.end();)
		{
			if ((*it)->GetName() == name)
			{
				// 제거 함수?
				(*it).reset();
				it = m_gameObjects.erase(it);
				return;
			}
			else
				it++;
		}
	}

	void Scene::RemoveGameObject(GameObject* target)
	{
		auto it = m_gameObjects.begin();
		for (it; it != m_gameObjects.end();)
		{
			if ((*it)->GetID() == target->GetID())
			{
				// 제거 함수?
				(*it).reset();
				it = m_gameObjects.erase(it);
				return;
			}
			else
				it++;
		}
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
}