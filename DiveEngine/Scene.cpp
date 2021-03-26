#include "Scene.h"
#include "GameObject.h"
#include "Log.h"

using namespace std;

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

	GameObject* Scene::CreateGameObject()
	{
		auto newGameObject = m_gameObjects.emplace_back(make_shared<GameObject>());
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
}