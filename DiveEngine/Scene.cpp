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
		m_name = "Untitled";
		m_bDirty = false;
	}

	Scene::~Scene()
	{
		Clear();
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
		if (!m_gameObjects.empty())
		{
			m_gameObjects.clear();
		}

		m_bDirty = true;
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

		for (const auto& pGameObject : rootGameObjects)
		{
			stream.Write(pGameObject->GetID());
		}
		for (const auto& pGameObject : rootGameObjects)
		{
			pGameObject->Serialize(&stream);
		}

		stream.Close();

		m_bDirty = false;

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
			auto pGameObject = CreateGameObject();
			pGameObject->SetID(stream.ReadAs<unsigned int>());
		}
		for (unsigned int i = 0; i != rootCount; i++)
		{
			m_gameObjects[i]->Deserialize(&stream, nullptr);
		}

		stream.Close();

		// ���� ���θ� �˷��� �Ѵ�.

		return true;
	}

	GameObject* Scene::CreateGameObject()
	{
		auto newGameObject = m_gameObjects.emplace_back(std::make_shared<GameObject>());
		// ���� ȣ��?

		m_bDirty = true;

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

		// ���ĸ�ź�� �ٷ� ������ �ʴ´�. �ϴ� ���� ������� �����س��´�.
		// �׸��� ���� Frame���� �����Ѵ�.

		m_bDirty = true;

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
		// �ڽĵ���� ���� ����
		auto pChildren = pGameObject->GetTransform()->GetChildren();
		for (auto pChild : pChildren)
		{
			gameObjectRemove(pChild->GetOwner());
		}

		auto pParent = pGameObject->GetTransform()->GetParent();

		// �θ𿡰Լ� ���ܵǴ� ������ ����.
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

		m_bDirty = true;
	}
}