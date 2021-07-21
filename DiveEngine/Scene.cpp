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
		m_bChanged = false;
	}

	Scene::Scene(const std::string& sceneName)
		: Object(typeid(Scene).hash_code())
	{
		SetName(sceneName);

		m_bChanged = false;
		// �� �ָ����.
		m_bLoaded = true;
	}

	Scene::~Scene()
	{
		Clear();
	}

	void Scene::Update(float deltaTime)
	{
		// ������ �ָ��ϳ�...
		// Dirty Check�� ���� �ƴѰ�?
		for (auto& gameObject : m_GameObjects)
		{
			gameObject->Update(deltaTime);
		}

		if (m_bChanged)
		{
			// ���� ���������� ���ҽ��� �����Ѵ�.
			auto copy = m_GameObjects;

			// �� �߿��� ������ �̰� �� �������� ���ذ� �ȵȴ�.
			// ���� ����� gameObject�� �����ϱ� ������ �� ����.
			// �׷��� swap�� ���� �ʴ´ٸ� ���� copy�� ó���� �ʿ䰡 �ֳ�?
			for (auto gameObject : copy)
			{
				if(gameObject->IsPendingDestruction())
				{
					eraseGameObject(gameObject);
				}
			}

			EVENT_FIRE(eEventType::SceneResolve);
			m_bChanged = false;
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

		m_bChanged = true;
	}

	bool Scene::SaveToFile(const std::string& filepath)
	{
		// ��θ� �޴´�.
		// ������ �⺻ ��θ� ����Ѵ�.
		// �̸� + .scene Ȯ���ڷ� �����Ѵ�.
		// ��, �̸��� �־�� �Ѵ�.
		// �ٸ� �̸����� ������ �� �־�� �Ѵ�.
		// ���� Scene�� �̸��� ������ �̸��� �ٸ� �� �ִ�.


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

		// ������ �̸��� Ȯ���ڱ��� ���Ե� ��θ� �޴´�.

		if (!FileSystemHelper::FileExists(filepath))
		{
			return false;
		}

		FileStream stream(filepath, eFileStreamMode::Read);
		if (!stream.IsOpen())
			return false;

		// id �������� �ݵ�� �ʱ�ȭ�� �ʿ��ϴ�.
		Clear();

		// ��Ʈ ���ӿ�����Ʈ�� ������ �� ������ȭ ����
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

		m_bChanged = true;
		m_bLoaded = true;

		return true;
	}

	//==================================//
	// �߰����� ó���� �ʿ��� �� �ִ�.	//
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

	// ���޹��� ����� �ڽĵ���� ��� ���� ���
	void Scene::RemoveGameObject(GameObject* pGameObjct)
	{
		if (!pGameObjct)
			return;

		pGameObjct->MarkForDestruction();

		auto transform = pGameObjct->GetTransform();
		assert(transform);

		auto children = transform->GetChildren();
		for (auto child : children)
		{
			RemoveGameObject(child->GetGameObject());
		}
		
		m_bChanged = true;
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

	void Scene::eraseGameObject(GameObject* pGameObjct)
	{
		if (!pGameObjct)
			return;

		pGameObjct->GetTransform()->SetParent(nullptr);
		pGameObjct->GetTransform()->DetachChildren();

		for (auto it = m_GameObjects.begin(); it != m_GameObjects.end();)
		{
			if ((*it)->GetInstanceID() == pGameObjct->GetInstanceID())
			{
				delete (*it);
				(*it) = nullptr;

				m_GameObjects.erase(it);
				break;
			}
			else
				++it;
		}
	}
}