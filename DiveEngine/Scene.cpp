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

	GameObject* Scene::CreateGameObject()
	{
		auto newGameObject = m_gameObjects.emplace_back(make_shared<GameObject>());
		// ���� ȣ��?

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
				// ���� �Լ�?
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
				// ���� �Լ�?
				(*it).reset();
				it = m_gameObjects.erase(it);
				return;
			}
			else
				it++;
		}
	}
}