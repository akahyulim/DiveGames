#include "Scene.h"
#include "GameObject.h"
#include "Log.h"
#include "Transform.h"
#include "FileSystemHelper.h"
#include "FileStream.h"

namespace dive
{
	Scene::Scene()
		: Object(typeid(Scene).hash_code())
	{
		mName = "Untitled";
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
	}

	void Scene::Clear()
	{
		if (!mGameObjects.empty())
		{
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
			stream.Write(gameObject->GetID());
		}
		for (const auto& gameObject : rootGameObjects)
		{
			gameObject->Serialize(&stream);
		}

		stream.Close();

		mbDirty = false;

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
			gameObject->SetID(stream.ReadAs<unsigned int>());
		}
		for (unsigned int i = 0; i != rootCount; i++)
		{
			mGameObjects[i]->Deserialize(&stream, nullptr);
		}

		stream.Close();

		// ���� ���θ� �˷��� �Ѵ�.

		return true;
	}

	GameObject* Scene::CreateGameObject()
	{
		auto pAddedObject = mGameObjects.emplace_back(std::make_shared<GameObject>());
		// ���� ȣ��?

		mbDirty = true;

		return pAddedObject.get();
	}

	GameObject* Scene::GetGameObjectByName(const std::string& name)
	{
		for (auto& target : mGameObjects)
		{
			if (target->GetName() == name)
				return target.get();
		}

		return nullptr;
	}

	GameObject* Scene::GetGameObjectByID(unsigned int id)
	{
		for (auto& pTarget : mGameObjects)
		{
			if (pTarget->GetID() == id)
				return pTarget.get();
		}

		return nullptr;
	}

	void Scene::RemoveGameObject(GameObject* target)
	{
		assert(target != nullptr);

		// ���ĸ�ź�� �ٷ� ������ �ʴ´�. �ϴ� ���� ������� �����س��´�.
		// �׸��� ���� Frame���� �����Ѵ�.

		mbDirty = true;

		gameObjectRemove(target);
	}

	std::vector<GameObject*> Scene::GetRootGameObjects()
	{
		std::vector<GameObject*> rootGameObjects;
		for (auto gameObject : mGameObjects)
		{
			auto pTransform = gameObject->GetComponent<Transform>();
			if (pTransform)
			{
				if (!pTransform->HasParent())
				{
					rootGameObjects.emplace_back(gameObject.get());
				}
			}
		}

		return rootGameObjects;
	}

	void Scene::gameObjectRemove(GameObject* gameObject)
	{
		// �ڽĵ���� ���� ����
		auto pChildren = gameObject->GetTransform()->GetChildren();
		for (auto pChild : pChildren)
		{
			gameObjectRemove(pChild->GetOwner());
		}

		auto parentTransform = gameObject->GetTransform()->GetParent();

		// �θ𿡰Լ� ���ܵǴ� ������ ����.
		for (auto it = mGameObjects.begin(); it != mGameObjects.end();)
		{
			auto pTarget = *it;
			if (pTarget->GetID() == gameObject->GetID())
			{
				it = mGameObjects.erase(it);
				break;
			}
			++it;
		}

		if (parentTransform)
		{
			parentTransform->AcquireChidren();
		}

		mbDirty = true;
	}
}