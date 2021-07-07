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
		mbDirty = false;
	}

	Scene::~Scene()
	{
		Clear();
	}

	void Scene::Update(float deltaTime)
	{
		for (auto& gameObject : mGameObjects)
		{
			gameObject->Update(deltaTime);
		}

		if (mbDirty)
		{
			// ���� ���������� ���ҽ��� �����Ѵ�.
			auto copy = mGameObjects;

			// �� �߿��� ������ �̰� �� �������� ���ذ� �ȵȴ�.
			for (auto gameObject : copy)
			{
				if(gameObject->IsPendingDestruction())
				{
					eraseGameObject(gameObject);
				}
			}

			EVENT_FIRE(eEventType::SceneResolve);
			mbDirty = false;
		}
	}

	void Scene::Clear()
	{
		if (!mGameObjects.empty())
		{
			for (auto gameObject : mGameObjects)
			{
				delete gameObject;
				gameObject = nullptr;
			}

			mGameObjects.clear();
			mGameObjects.shrink_to_fit();
		}

		mbDirty = true;
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

		auto rootCount = stream.ReadAs<unsigned int>();
		for (unsigned int i = 0; i != rootCount; i++)
		{
			auto gameObject = CreateGameObject();
			gameObject->SetInstanceID(stream.ReadAs<unsigned int>());
		}
		for (unsigned int i = 0; i != rootCount; i++)
		{
			mGameObjects[i]->Deserialize(&stream, nullptr);
		}

		stream.Close();

		mbDirty = true;

		return true;
	}

	//==================================//
	// �߰����� ó���� �ʿ��� �� �ִ�.	//
	//==================================//
	GameObject* Scene::CreateGameObject(bool active)
	{
		auto newGameObject = mGameObjects.emplace_back(new GameObject);
		newGameObject->SetActive(active);

		return newGameObject;
	}

	GameObject* Scene::GetGameObjectByName(const std::string& name)
	{
		for (auto gameObject : mGameObjects)
		{
			if (gameObject->GetName() == name)
				return gameObject;
		}

		return nullptr;
	}

	GameObject* Scene::GetGameObjectByID(unsigned int id)
	{
		for (auto gameObject : mGameObjects)
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

		mbDirty = true;
	}

	std::vector<GameObject*> Scene::GetRootGameObjects() const
	{
		std::vector<GameObject*> rootGameObjects;
		for (auto gameObject : mGameObjects)
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
	// ���޹��� ����� �ڽĵ���� ��� �����մϴ�.	//
	//==============================================//
	void Scene::eraseGameObject(GameObject* gameObject)
	{
		if (!gameObject)
			return;

		auto children = gameObject->GetTransform()->GetChildren();
		for (auto child : children)
		{
			RemoveGameObject(child->GetGameObject());
		}

		auto parent = gameObject->GetTransform()->GetParent();

		for (auto it = mGameObjects.begin(); it != mGameObjects.end();)
		{
			if ((*it)->GetInstanceID() == gameObject->GetInstanceID())
			{
				delete (*it);
				(*it) = nullptr;
				it = mGameObjects.erase(it);
				break;
			}
			++it;
		}

		if (parent)
		{
			parent->AcquireChidren();
		}
	}
}