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

		// ���⿡�� �������� LoadedScene�� �������� �ʿ䰡 �ֳĴ� �Ŵ�.
		// swap���� �ʿ��ϴ��ص� 3�� �̻��� �ʿ�� ���ݾ�?
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
	
	// �̰� ��� SetActiveScene���� ����?
	Scene* SceneManager::CreateScene(const std::string& sceneName)
	{
		if (!m_LoadedScene.empty())
		{
			for (auto scene : m_LoadedScene)
			{
				if (scene->GetName() == sceneName)
				{
					CORE_WARN("SceneManager::CreateScene >> �̹� �����ϴ� Scene Name({:s})�� ���޹޾� ������ ��ҵǾ����ϴ�.", sceneName);
					return nullptr;
				}
			}
		}

		auto newScene = m_LoadedScene.emplace_back(new Scene(sceneName));

		return newScene;
	}

	//==========================================//
	// Load���� ���� Scene�� ����� �� ����.	//
	// ������� ������ �� ����.					//
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

	// ���� ��� Scene ��ȯ
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