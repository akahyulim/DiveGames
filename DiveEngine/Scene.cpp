#include "Scene.h"
#include "GameObject.h"
#include "Log.h"
#include "Transform.h"
#include "FileSystemHelper.h"
#include "FileStream.h"

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
		for (auto& gameObject : m_gameObjects)
		{
			gameObject->Update(deltaTime);
		}
	}

	void Scene::Clear()
	{
		m_gameObjects.clear();
	}

	bool Scene::SaveToFile(const std::string& filepath)
	{
		// 경로를 받는다.
		// 없으면 기본 경로를 사용한다.
		// 이름 + .scene 확장자로 관리한다.
		// 즉, 이름이 있어야 한다.
		// 다른 이름으로 저장할 수 있어야 한다.
		// 따라서 Scene의 이름과 파일의 이름이 다를 수 있다.


		FileStream stream(filepath, eFileStreamMode::Write);
		if (!stream.IsOpen())
		{
			CORE_ERROR("");
			return false;
		}

		auto rootGameObjects = GetRootGameObjects();
		auto rootCount = static_cast<unsigned int>(rootGameObjects.size());
		stream.Write(rootCount);

		for (const auto& pGameObject : rootGameObjects)
		{
			stream.Write(pGameObject->GetID());
		}
		for (const auto& pGameObject : rootGameObjects)
		{
			pGameObject->Serialize(&stream);
		}

		stream.Close();

		return true;
	}

	bool Scene::LoadFromFile(const std::string& filepath)
	{
		// 이쪽은 이름과 확장자까지 포함된 경로를 받는다.

		if (!FileSystemHelper::FileExists(filepath))
		{
			CORE_ERROR("");
			return false;
		}

		
		FileStream stream(filepath, eFileStreamMode::Read);
		if (!stream.IsOpen())
			return false;

		// id 때문에라도 반드시 초기화가 필요하다.
		Clear();

		auto rootCount = stream.ReadAs<unsigned int>();
		for (unsigned int i = 0; i != rootCount; i++)
		{
			auto pGameObject = CreateGameObject();
			pGameObject->SetID(stream.ReadAs<unsigned int>());
		}
		for (unsigned int i = 0; i != rootCount; i++)
		{
			m_gameObjects[i]->Deserialize(&stream, nullptr);
		}

		stream.Close();

		// 갱신 여부를 알려야 한다.

		return true;
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