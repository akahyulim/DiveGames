#include "divepch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	SceneManager* SceneManager::s_pInstance = nullptr;

	SceneManager::SceneManager()
		: m_pActiveScene(nullptr)
	{
	}

	SceneManager::~SceneManager()
	{
		Shutdown();
	}

	void SceneManager::Shutdown()
	{
		for (auto pScene : m_Scenes)
		{
			pScene->Clear();
			DV_DELETE(pScene);
		}

		m_pActiveScene = nullptr;

		DV_ENGINE_TRACE("씬 매니져 셧다운에 성공하였습니다.");
	}

	Scene* SceneManager::CreateScene(const std::string& sceneName)
	{
		for (auto pScene : m_Scenes)
		{
			if (pScene->GetName() == sceneName)
			{
				DV_ENGINE_WARN("동일한 이름의 씬({:s})이 이미 존재합니다.", sceneName);
				return nullptr;
			}
		}

		return m_Scenes.emplace_back(new Scene((int)m_Scenes.size(), sceneName));
	}

	void SceneManager::LoadScene(const std::string& sceneName)
	{
		Scene* pScene = GetSceneByName(sceneName);
		if (pScene)
			pScene->LoadFromFile(pScene->GetPath());
	}

	void SceneManager::LoadScene(int index)
	{
		if (index < 0 || index >= m_Scenes.size())
		{
			DV_ENGINE_WARN("잘못된 씬 인덱스({:d})를 전달받았습니다.", index);
			return;
		}

		Scene* pScene = GetSceneAt(index);
		if (pScene)
			pScene->LoadFromFile(pScene->GetPath());
	}

	bool SceneManager::UnloadScene(const std::string& sceneName)
	{
		Scene* pScene = GetSceneByName(sceneName);
		if (!pScene)
			return false;

		if (pScene->IsLoaded())
			pScene->Clear();

		return true;
	}

	bool SceneManager::UnloadScene(int index)
	{
		if (index < 0 || index >= m_Scenes.size())
		{
			DV_ENGINE_WARN("잘못된 씬 인덱스({:d})를 전달받았습니다.", index);
			return false;
		}

		Scene* pScene = GetSceneAt(index);
		if (!pScene)
			return false;

		if (pScene->IsLoaded())
			pScene->Clear();

		return true;
	}

	Scene* SceneManager::GetActiveScene()
	{
		return m_pActiveScene;
	}

	bool SceneManager::SetActiveScene(Scene* pScene)
	{
		if (!GetSceneAt(pScene->GetIndex()))
			return false;

		// 유니티에선 로드된 씬만 액티브씬이 될 수 있다.
		// 추후 엔진을 런타임, 에디터로 나누어야 할 수 있다는 말이다.
		//if (!pScene->IsLoaded())
		//	return false;

		m_pActiveScene = pScene;

		return true;
	}

	Scene* SceneManager::GetSceneAt(int index)
	{
		if (index < 0 || index >= m_Scenes.size())
		{
			DV_ENGINE_WARN("잘못된 씬 인덱스({:d})를 전달받았습니다.", index);
			return nullptr;
		}

		return m_Scenes[index];
	}

	Scene* SceneManager::GetSceneByName(const std::string& sceneName)
	{
		for (auto pScene : m_Scenes)
		{
			if (pScene->GetName() == sceneName)
				return pScene;
		}

		return nullptr;
	}

	Scene* SceneManager::GetSceneByPath(const std::string& scenePath)
	{
		for (auto pScene : m_Scenes)
		{
			if (pScene->GetPath() == scenePath)
				return pScene;

		}
		return nullptr;
	}

	uint32_t SceneManager::GetSceneCount()
	{
		return static_cast<uint32_t>(m_Scenes.size());
	}

	SceneManager* GetSceneManager()
	{
		return SceneManager::GetInstance();
	}
}