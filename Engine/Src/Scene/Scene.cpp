#include "DivePch.h"
#include "Scene.h"
#include "SceneEvents.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "Core/CoreEvents.h"


namespace Dive
{
	Scene::Scene(Context * context, std::string name)
		: Object(context),
		m_bDirty(false)
	{
		if (name.empty())
			m_name = "No Name";
		else
			m_name = std::move(name);

		DIVE_SUBSCRIBE_TO_EVENT(this, &Scene::OnUpdate);
		CORE_TRACE("Create Scene - {:s}", m_name);
	}

	Scene::~Scene()
	{
		Unload();

		DIVE_UNSUBSCRIBE_FROM_EVENT(Scene, E_UPDATE, this);
		CORE_TRACE("Destroy Scene - {:s} ===========================", m_name);
	}

	void Scene::OnUpdate(const E_UPDATE* evnt)
	{
		if (!evnt)
			return;

		// game object들을 실행한다.
		{
			bool start = false;// 게임 모드 && 에디터 모드면 start
			bool stop = false;// 둘 다 아니면 stop
			
			if (start)
			{
				for (const auto& gameObject : m_gameObjects)
				{
					gameObject->Start();
				}
			}

			if (stop)
			{
				for (const auto& gameObject : m_gameObjects)
				{
					gameObject->Stop();
				}
			}

			//tick
			for (const auto& gameObject : m_gameObjects)
			{
				gameObject->Update();
			}
		}

		if (IsDirty())
		{
			{
				auto copyGameObjects = m_gameObjects;
				for (auto& gameObject : copyGameObjects)
				{
					if (gameObject->IsPendingDestruction())
					{
						removeGameObject(gameObject);
					}
				}
			}

			E_UPDATE_SCENE evnt(m_gameObjects);
			DIVE_FIRE_EVENT(&evnt);
			m_bDirty = false;
		}
	}

	void Scene::Unload()
	{
		m_gameObjects.clear();
		m_gameObjects.shrink_to_fit();

		m_bDirty = true;
	}

	std::shared_ptr<GameObject> Scene::CreateGameObject()
	{
		m_bDirty = true;
		return m_gameObjects.emplace_back(std::make_shared<GameObject>(m_context));
	}

	void Scene::RemoveGameObject(const std::shared_ptr<GameObject>& gameObject)
	{
		if (!gameObject)
			return;

		if (!ExistsGameObject(gameObject))
			return;

		gameObject->MarkForDestruction();
		m_bDirty = true;
	}

	const std::shared_ptr<GameObject>& Scene::GetGameObjectByName(const std::string& name)
	{
		for (const auto& gameObject : m_gameObjects)
		{
			if (gameObject->GetName() == name)
				return gameObject;
		}

		static std::shared_ptr<GameObject> empty;
		return empty;
	}

	const std::shared_ptr<GameObject>& Scene::GetGameObjectByID(unsigned int id)
	{
		for (const auto& gameObject : m_gameObjects)
		{
			if (gameObject->GetID() == id)
				return gameObject;
		}

		static std::shared_ptr<GameObject> empty;
		return empty;
	}

	bool Scene::ExistsGameObject(const std::shared_ptr<GameObject>& gameObject)
	{
		if (!gameObject)
			return false;

		return GetGameObjectByID(gameObject->GetID()) != nullptr;
	}

	std::vector<std::shared_ptr<GameObject>> Scene::GetRootGameObjects()
	{
		std::vector<std::shared_ptr<GameObject>> targetObjects;
		for (const auto& gameObject : m_gameObjects)
		{
			auto transform = gameObject->GetComponent<Transform>();
			if (transform)
			{
				if (transform->IsRoot())
					targetObjects.emplace_back(gameObject);
			}
		}

		return targetObjects;
	}

	void Scene::removeGameObject(const std::shared_ptr<GameObject> gameObject)
	{
		// 자손들부터 제거
		auto children = gameObject->GetComponent<Transform>()->GetChildren();
		for (const auto& child : children)
		{
			removeGameObject(child->GetOwner()->GetSharedPtr());
		}

		// 계층구조에서 제거
		auto it = m_gameObjects.begin();
		for (it; it != m_gameObjects.end();)
		{
			if ((*it)->GetID() == gameObject->GetID())
			{
				(*it).reset();
				it = m_gameObjects.erase(it);
				break;
			}
			else
				++it;
		}

		// 부모로부터 제거
		auto parent = gameObject->GetComponent<Transform>()->GetParent();
		if (parent)
		{
			// 부모의 계층 관계 업데이트
			parent->AcquireChildern();
		}
	}
}