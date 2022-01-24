#include "divepch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "../Core/Runtime.h"
#include "../Core/Event.h"
#include "../Helper/FileSystemHelper.h"
#include "../Helper/Log.h"

namespace Dive
{
	SceneManager::SceneManager()
	{
	}

	SceneManager::~SceneManager()
	{
		m_pActiveScene = nullptr;

		// 여기에서 문제점은 LoadedScene이 여러개일 필요가 있냐는 거다.
		// swap용이 필요하다해도 3개 이상일 필요는 없잖아?
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
	
	// 이건 어떻게 SetActiveScene으로 하지?
	Scene* SceneManager::CreateScene(const std::string& sceneName)
	{
		if (!m_LoadedScene.empty())
		{
			for (auto scene : m_LoadedScene)
			{
				if (scene->GetName() == sceneName)
				{
					CORE_WARN("SceneManager::CreateScene >> 이미 존재하는 Scene Name({:s})을 전달받아 생성이 취소되었습니다.", sceneName);
					return nullptr;
				}
			}
		}

		auto newScene = m_LoadedScene.emplace_back(new Scene(sceneName));

		return newScene;
	}

	//==========================================//
	// Load되지 않은 Scene은 사용할 수 없다.	//
	// 여러모로 생각할 게 많다.					//
	//==========================================//
	bool SceneManager::SetActiveScene(Scene* pScene)
	{
		assert(pScene && "INVALID_PARAMETER");
		
		if (pScene->IsLoaded())
		{
			m_pActiveScene = pScene;

			EVENT_FIRE(eEventType::SceneActivate);

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

	// 동기 방식 Scene 전환
	Scene* SceneManager::LoadScene(const std::string& scenePath)
	{
		if (FileSystemHelper::FileExists(scenePath))
		{
			auto sceneName = FileSystemHelper::GetFilenameWithoutExtension(scenePath);
			auto newScene = CreateScene(sceneName);
			if (!newScene)
				return nullptr;

			m_pActiveScene = newScene;

			EVENT_FIRE(eEventType::SceneActivate);

			if (!newScene->LoadFromFile(scenePath))
				return nullptr;

			return newScene;
		}
	
		return nullptr;
	}
}