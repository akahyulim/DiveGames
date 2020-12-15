#include "DivePch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Core/Context.h"
#include "Core/Log.h"


namespace Dive
{
	Scene::Scene(Context * context, std::string name)
		: Object(context),
		m_bDirty(false)
	{
		if (name.empty())
			m_name = "Scene";
		else
			m_name = std::move(name);

		CORE_TRACE("Create Scene - {:s}", m_name);
	}

	Scene::~Scene()
	{
		Unload();

		CORE_TRACE("Destroy Scene - {:s}", m_name);
	}

	bool Scene::SaveToFile(const std::string & filename)
	{
		return true;
	}

	bool Scene::LoadFromFile(const std::string & filename)
	{

		m_bDirty = true;

		return true;
	}

	void Scene::Unload()
	{
		m_gameObjects.clear();
		m_gameObjects.shrink_to_fit();

		m_bDirty = true;
	}

	std::shared_ptr<GameObject>& Scene::CreateGameObject(const std::string& name)
	{
		// 이름으로 검색이 필요하다.

		auto gameObject =  m_gameObjects.emplace_back(std::make_shared<GameObject>(m_context, name));

		m_bDirty = true;

		return gameObject;
	}

	void Scene::AddGameObject(const std::shared_ptr<GameObject>& gameObject)
	{
		if (!gameObject)
			return;

		// 역시 동일한 객체가 이미 존재하는지 확인이 필요하다.

		m_gameObjects.emplace_back(gameObject);

		m_bDirty = true;
	}

	// 렌더링 중일 수 있기에 바로 제거하면 안된다.
	void Scene::RemoveGameObject(const std::shared_ptr<GameObject>& gameObject)
	{
		if (!gameObject)
			return;

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
		// root gameObject들로 vector를 꾸려 전달한다.
		// 이를 위해선 transform이 필요하다.

		return std::vector<std::shared_ptr<GameObject>>();
	}
}