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
					CORE_WARN("SceneManager::CreateScene >> 이미 존재하는 Scene Name을 전달받아 생성이 취소되었습니다.");
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
			// swap 과정이 필요한가?
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

	// 유니티 예제를 보면 Scene1과 Scene2 그리고 ProgressScene 총 세개가 존재한다.
	// 문제는 Scene을 제거해야 하느냐이다.
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