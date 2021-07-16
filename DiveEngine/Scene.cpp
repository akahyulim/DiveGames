#include "Scene.h"
#include "GameObject.h"
#include "Log.h"
#include "Transform.h"
#include "FileSystemHelper.h"
#include "FileStream.h"
#include "Event.h"

namespace dive
{
	Scene::Scene()
		: Object(typeid(Scene).hash_code())
	{
		SetName("Untitled");
		m_bDirty = false;
	}

	Scene::Scene(const std::string& sceneName)
		: Object(typeid(Scene).hash_code())
	{
		SetName(sceneName);

		m_bDirty = false;
		// 좀 애매허다.
		m_bLoaded = true;
	}

	Scene::~Scene()
	{
		Clear();
	}

	void Scene::Update(float deltaTime)
	{
		// 순서가 애매하네...
		// Dirty Check가 먼저 아닌가?
		for (auto& gameObject : m_GameObjects)
		{
			gameObject->Update(deltaTime);
		}

		if (m_bDirty)
		{
			// 얕은 복사이지만 리소스를 차지한다.
			auto copy = m_GameObjects;

			// 더 중요한 문제는 이게 왜 먹히는지 이해가 안된다.
			// 얕은 복사라서 gameObject는 동일하기 때문인 것 같다.
			// 그런데 swap도 하지 않는다면 굳이 copy로 처리할 필요가 있나?
			for (auto gameObject : copy)
			{
				if(gameObject->IsPendingDestruction())
				{
					eraseGameObject(gameObject);
				}
			}

			EVENT_FIRE(eEventType::SceneResolve);
			m_bDirty = false;
		}
	}

	void Scene::Clear()
	{
		if (!m_GameObjects.empty())
		{
			for (auto gameObject : m_GameObjects)
			{
				delete gameObject;
				gameObject = nullptr;
			}

			m_GameObjects.clear();
			m_GameObjects.shrink_to_fit();
		}

		m_bDirty = true;
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

		for (const auto& gameObject : rootGameObjects)
		{
			stream.Write(gameObject->GetInstanceID());
		}
		for (const auto& gameObject : rootGameObjects)
		{
			gameObject->Serialize(&stream);
		}

		stream.Close();

		return true;
	}

	bool Scene::LoadFromFile(const std::string& filepath)
	{
		m_bLoaded = false;

		// 이쪽은 이름과 확장자까지 포함된 경로를 받는다.

		if (!FileSystemHelper::FileExists(filepath))
		{
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
			auto gameObject = CreateGameObject();
			gameObject->SetInstanceID(stream.ReadAs<unsigned int>());
		}
		for (unsigned int i = 0; i != rootCount; i++)
		{
			m_GameObjects[i]->Deserialize(&stream, nullptr);
		}

		stream.Close();

		m_bDirty = true;
		m_bLoaded = true;

		return true;
	}

	//==================================//
	// 추가적인 처리가 필요할 수 있다.	//
	//==================================//
	GameObject* Scene::CreateGameObject(bool active)
	{
		auto newGameObject = m_GameObjects.emplace_back(new GameObject(this));
		newGameObject->SetActive(active);

		return newGameObject;
	}

	GameObject* Scene::GetGameObjectByName(const std::string& name)
	{
		for (auto gameObject : m_GameObjects)
		{
			if (gameObject->GetName() == name)
				return gameObject;
		}

		return nullptr;
	}

	GameObject* Scene::GetGameObjectByID(unsigned int id)
	{
		for (auto gameObject : m_GameObjects)
		{
			if (gameObject->GetInstanceID() == id)
				return gameObject;
		}

		return nullptr;
	}

	void Scene::RemoveGameObject(GameObject* gameObject)
	{
		if (!gameObject)
			return;

		gameObject->MarkForDestruction();

		m_bDirty = true;
	}

	std::vector<GameObject*> Scene::GetRootGameObjects() const
	{
		std::vector<GameObject*> rootGameObjects;
		for (auto gameObject : m_GameObjects)
		{
			auto pTransform = gameObject->GetComponent<Transform>();
			if (pTransform)
			{
				if (!pTransform->HasParent())
				{
					rootGameObjects.emplace_back(gameObject);
				}
			}
		}

		return rootGameObjects;
	}

	//==============================================//
	// 전달받은 대상의 자식들까지 모두 제거합니다.	//
	//==============================================//
	void Scene::eraseGameObject(GameObject* gameObject)
	{
		assert(gameObject && "INVALID PARAMETER");

		if (!gameObject)
		{
			CORE_WARN("Scene::eraseGameObject>> 잘못된 인자를 전달받았습니다.");
			return;
		}

		// 제거될 대상의 자식들 역시 제거 대상으로 설정한다.
		auto children = gameObject->GetTransform()->GetChildren();
		for (auto child : children)
		{
			RemoveGameObject(child->GetGameObject());
		}

		// 지워질 대상의 부모를 우선 저장한다.
		auto parent = gameObject->GetTransform()->GetParent();

		// 현재 GameObject를 컨테이너에서 제거한다.
		// 그렇다면 위에서 제거 대상으로 추가 설정한 자식들은 언제 제거할까?
		for (auto it = m_GameObjects.begin(); it != m_GameObjects.end();)
		{
			if ((*it)->GetInstanceID() == gameObject->GetInstanceID())
			{
				delete (*it);
				(*it) = nullptr;
				it = m_GameObjects.erase(it);
				break;
			}
			++it;
		}

		// 제거된 대상의 부모가 자식을 다시 설정토록 한다.
		if (parent)
		{
			parent->AcquireChidren();
		}
	}
}