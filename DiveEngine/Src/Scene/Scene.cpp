#include "DivePch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	static const uint64_t FIRST_ID = 0x1;
	static const uint64_t LAST_ID = 0xffffffffffffffff;

	static std::string s_SceneName = "Empty World";
	static std::string s_SceneFilepath;

	static std::unordered_map<uint64_t, GameObject*> s_GameObjects;
	static std::unordered_map<uint64_t, Component*> s_Components;

	static uint64_t s_CurGameObjectID = FIRST_ID;
	static uint64_t s_CurComponentID = FIRST_ID;

	static bool s_bDirty = false;

	bool Scene::Initialize()
	{
		DV_CORE_TRACE("Scene::Initialize()");
		return true;
	}

	void Scene::Shutdown()
	{
		Clear();

		DV_CORE_TRACE("Scene::Shutdown() - {:s}", GetName());
	}

	void Scene::New()
	{
		DV_CORE_TRACE("Scene::New() - {:s}", GetName());
	}

	void Scene::Clear()
	{
		auto it = s_GameObjects.begin();
		for (it; it != s_GameObjects.end(); ++it)
			DV_DELETE(it->second);
		s_GameObjects.clear();
		s_Components.clear();

		s_CurGameObjectID = FIRST_ID;
		s_CurComponentID = FIRST_ID;
	}

	void Scene::Update(float delta)
	{
		for (auto pGameObject : s_GameObjects)
			pGameObject.second->Update(delta);

		if (s_bDirty)
		{
			auto it = s_GameObjects.begin();
			for (it; it != s_GameObjects.end();)
			{
				if (it->second->IsRemovedTarget())
				{
					DV_DELETE(it->second);
					it = s_GameObjects.erase(it);
				}
				else
					it++;
			}

			s_bDirty = false;
		}
	}

	bool Scene::LoadFromFile(const std::string& filePath)
	{
		DV_CORE_TRACE("Scene::LoadFromFile() - {:s}", filePath);
		return true;
	}

	bool Scene::SaveToFile(const std::string& filePath)
	{
		DV_CORE_TRACE("Scene::SaveToFile() - {:s}", filePath);
		return true;
	}

	std::string Scene::GetName()
	{
		DV_CORE_TRACE("Scene::GetName() - {:s}", s_SceneName);
		
		return s_SceneName;
	}

	std::string Scene::GetFilepath()
	{
		DV_CORE_TRACE("Scene::GetFilepath()");
		return std::string("");
	}

	GameObject* Scene::CreateGameObject(const std::string& name)
	{
		auto id = getFreeGameObjectID();
		if (id == 0)
		{
			DV_CORE_ERROR("더이상 새로운 GameObjecrt를 생성할 수 없습니다.");
			return nullptr;
		}

		auto pNewGameObject = new GameObject(name);
		DV_ASSERT(pNewGameObject);

		s_GameObjects[id] = pNewGameObject;
		pNewGameObject->SetID(id);

		return pNewGameObject;
	}

	void Scene::RemoveGameObject(GameObject* pGameObject)
	{
		if (!pGameObject || s_GameObjects.empty())
			return;

		RemoveGameObjectByID(pGameObject->GetID());
	}

	void Scene::RemoveGameObjectByID(uint64_t id)
	{
		if (!id || s_GameObjects.empty())
			return;
	
		if (!s_bDirty)
			s_bDirty = true;

		auto it = s_GameObjects.begin();
		for (it; it != s_GameObjects.end(); ++it)
		{
			if (it->first == id)
			{
				auto* pTransform = it->second->GetComponent<Transform>();
				if (pTransform)
				{
					if (pTransform->HasChild())
					{
						for (auto* pChild : pTransform->GetChildren())
							RemoveGameObjectByID(pChild->GetGameObject()->GetID());
					}

					if (pTransform->HasParent())
						pTransform->SetParent(nullptr);
				}

				it->second->MarkRemoveTarget();
			}
		}
	}

	GameObject* Scene::GetGameObjectByID(uint64_t id)
	{
		auto it = s_GameObjects.begin();
		return it != s_GameObjects.end() ? it->second : nullptr;
	}

	bool Scene::ExistsGameObject(GameObject* pGameObject)
	{
		if(!pGameObject)
			return false;

		return ExistsGameObjectByID(pGameObject->GetID());
	}

	bool Scene::ExistsGameObjectByID(uint64_t id)
	{
		auto it = s_GameObjects.find(id);
		return it != s_GameObjects.end();
	}

	std::vector<GameObject*> Scene::GetRootGameObjects()
	{
		std::vector<GameObject*> rootGameObjects;

		auto it = s_GameObjects.begin();
		for (it; it != s_GameObjects.end(); ++it)
		{
			auto pTransform = it->second->GetComponent<Transform>();
			if (!pTransform)
				continue;

			if (pTransform->HasParent())
				rootGameObjects.emplace_back(it->second);
		}

		return rootGameObjects;
	}

	std::vector<GameObject*> Scene::GetAllGameObjects()
	{
		std::vector<GameObject*> allGameObjects;

		auto it = s_GameObjects.begin();
		for (it; it != s_GameObjects.end(); ++it)
			allGameObjects.emplace_back(it->second);

		return allGameObjects;
	}

	uint64_t Scene::GetGameObjectsCount()
	{
		return static_cast<uint64_t>(s_GameObjects.size());
	}

	void Scene::RegisterComponent(Component* pComponent, uint64_t id)
	{
		if (!pComponent)
			return;

		if (id == 0 || GetComponent(id))
		{
			id = getFreeComponentID();
			if (id == 0)
			{
				DV_CORE_ERROR("더이상 Component를 등록할 수 없습니다.");
				return;
			}
		}

		s_Components[id] = pComponent;
		pComponent->SetID(id);
	}

	void Scene::DeregisterComponent(Component* pComponent)
	{
		if (!pComponent)
			return;

		DeregisterComponentByID(pComponent->GetID());
	}

	void Scene::DeregisterComponentByID(uint64_t id)
	{
		if (!GetComponent(id))
			return;

		if (s_Components.find(id) != s_Components.end())
			s_Components.erase(id);
	}

	Component* Scene::GetComponent(uint64_t id)
	{
		auto it = s_Components.find(id);
		return it != s_Components.end() ? it->second : nullptr;
	}

	uint64_t Scene::getFreeGameObjectID()
	{
		auto checkID = s_CurGameObjectID;

		for (;;)
		{
			auto freeID = s_CurGameObjectID;

			if (s_CurGameObjectID < LAST_ID)
				++s_CurGameObjectID;
			else
				s_CurGameObjectID = FIRST_ID;

			if (checkID == s_CurGameObjectID)
				return 0;
			else if (!s_GameObjects[freeID])
				return freeID;
		}
	}

	uint64_t Scene::getFreeComponentID()
	{
		auto checkID = s_CurComponentID;

		for (;;)
		{
			auto freeID = s_CurComponentID;

			if (s_CurComponentID < LAST_ID)
				++s_CurComponentID;
			else
				s_CurComponentID = FIRST_ID;

			if (checkID == s_CurComponentID)
				return 0;
			else if (!s_Components[freeID])
				return freeID;
		}
	}
}