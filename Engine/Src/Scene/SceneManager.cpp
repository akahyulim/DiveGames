#include "divepch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	static std::vector<Scene*> s_Scenes;
	static Scene* s_pActiveScene = nullptr;

	void SceneManager::Shutdown()
	{
		for (auto pScene : s_Scenes)
		{
			pScene->Clear();
			DV_DELETE(pScene);
		}
		s_Scenes.clear();
	}

	Scene* SceneManager::CreateScene(const std::string& sceneName)
	{
		for (auto pScene : s_Scenes)
		{
			if (pScene->GetName() == sceneName)
			{
				DV_CORE_WARN("������ �̸��� ��({:s})�� �̹� �����մϴ�.", sceneName);
				return nullptr;
			}
		}

		return s_Scenes.emplace_back(new Scene((int)s_Scenes.size(), sceneName));
	}

	void SceneManager::LoadScene(const std::string& sceneName)
	{
		Scene* pScene = GetSceneByName(sceneName);
		if (pScene)
			pScene->LoadFromFile(pScene->GetPath());
	}

	void SceneManager::LoadScene(int index)
	{
		if (index < 0 || index >= s_Scenes.size())
		{
			DV_CORE_WARN("�߸��� �� �ε���({:d})�� ���޹޾ҽ��ϴ�.", index);
			return;
		}

		Scene* pScene = GetSceneAt(index);
		if (pScene)
			pScene->LoadFromFile(pScene->GetPath());
	}

	bool SceneManager::UnloadScene(const std::string& sceneName)
	{
		Scene* pScene = GetSceneByName(sceneName);
		if(!pScene)
			return false;

		if(pScene->IsLoaded())
			pScene->Clear();

		return true;
	}

	bool SceneManager::UnloadScene(int index)
	{
		if (index < 0 || index >= s_Scenes.size())
		{
			DV_CORE_WARN("�߸��� �� �ε���({:d})�� ���޹޾ҽ��ϴ�.", index);
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
		return s_pActiveScene;
	}

	bool SceneManager::SetActiveScene(Scene* pScene)
	{
		if (!GetSceneAt(pScene->GetIndex()))
			return false;

		// ����Ƽ���� �ε�� ���� ��Ƽ����� �� �� �ִ�.
		// ���� ������ ��Ÿ��, �����ͷ� ������� �� �� �ִٴ� ���̴�.
		//if (!pScene->IsLoaded())
		//	return false;

		s_pActiveScene = pScene;

		return true;
	}
	
	Scene* SceneManager::GetSceneAt(int index)
	{
		if (index < 0 || index >= s_Scenes.size())
		{
			DV_CORE_WARN("�߸��� �� �ε���({:d})�� ���޹޾ҽ��ϴ�.", index);
			return nullptr;
		}

		return s_Scenes[index];
	}
	
	Scene* SceneManager::GetSceneByName(const std::string& sceneName)
	{
		for (auto pScene : s_Scenes)
		{
			if (pScene->GetName() == sceneName)
				return pScene;
		}

		return nullptr;
	}
	
	Scene* SceneManager::GetSceneByPath(const std::string& scenePath)
	{
		for (auto pScene : s_Scenes)
		{
			if (pScene->GetPath() == scenePath)
				return pScene;

		}
		return nullptr;
	}

	uint32_t SceneManager::GetSceneCount()
	{
		return static_cast<uint32_t>(s_Scenes.size());
	}
}