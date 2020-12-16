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
		// �̸����� �˻��� �ʿ��ϴ�.

		auto gameObject =  m_gameObjects.emplace_back(std::make_shared<GameObject>(m_context, name));

		m_bDirty = true;

		return gameObject;
	}

	void Scene::AddGameObject(const std::shared_ptr<GameObject>& gameObject)
	{
		if (!gameObject)
			return;

		// ���� ������ ��ü�� �̹� �����ϴ��� Ȯ���� �ʿ��ϴ�.

		m_gameObjects.emplace_back(gameObject);

		m_bDirty = true;
	}

	// ������ ���� �� �ֱ⿡ �ٷ� �����ϸ� �ȵȴ�.
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
		// root gameObject��� vector�� �ٷ� �����Ѵ�.
		// �̸� ���ؼ� transform�� �ʿ��ϴ�.

		return std::vector<std::shared_ptr<GameObject>>();
	}
}