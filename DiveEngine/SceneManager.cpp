#include "SceneManager.h"
#include "Runtime.h"
#include "Scene.h"
#include "FileSystemHelper.h"
#include "Log.h"

namespace dive
{
	SceneManager::SceneManager()
	{
	}

	SceneManager::~SceneManager()
	{
		if (!m_LoadedScene.empty())
		{
			for (auto scene : m_LoadedScene)
			{
				delete scene;
				scene = nullptr;
			}
			m_LoadedScene.clear();
		}
	}
	
	Scene* SceneManager::CreateScene(const std::string& sceneName)
	{
		if (!m_LoadedScene.empty())
		{
			for (auto scene : m_LoadedScene)
			{
				if (scene->GetName() == sceneName)
				{
					CORE_WARN("SceneManager::CreateScene >> �̹� �����ϴ� Scene Name�� ���޹޾� ������ ��ҵǾ����ϴ�.");
					return nullptr;
				}
			}
		}

		auto newScene = m_LoadedScene.emplace_back(new Scene(sceneName));

		if (!m_ActiveScene)
			SetActiveScene(newScene);

		return newScene;
	}

	bool SceneManager::SetActiveScene(Scene* scene)
	{
		assert(scene && "INVALID_PARAMETER");
		
		if (scene->IsLoaded())
		{
			// swap ������ �ʿ��Ѱ�?
			m_ActiveScene = scene;

			return true;
		}

		return false;
	}

	Scene* SceneManager::GetSceneByName(const std::string& sceneName)
	{
		for (auto scene : m_LoadedScene)
		{
			if (scene->GetName() == sceneName)
				return scene;
		}

		return nullptr;
	}

	// ����Ƽ ������ ���� Scene1�� Scene2 �׸��� ProgressScene �� ������ �����Ѵ�.
	// ������ Scene�� �����ؾ� �ϴ����̴�.
	Scene* SceneManager::LoadScene(const std::string& scenePath)
	{
		if (FileSystemHelper::FileExists(scenePath))
		{
			auto sceneName = FileSystemHelper::GetFilenameWithoutExtension(scenePath);
			auto newScene = CreateScene(sceneName);
			if (!newScene)
				return nullptr;

			if (!newScene->LoadFromFile(scenePath))
				return nullptr;

			return newScene;
		}
	
		return nullptr;
	}
}