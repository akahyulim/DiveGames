#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"
#include "SceneSerializer.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	Scene* SceneManager::s_pActiveScene = nullptr;

	Scene* SceneManager::CreateScene(const std::string& sceneName)
	{
		if (s_pActiveScene && s_pActiveScene->GetName() == sceneName)
		{
			DV_LOG(SceneManager, warn, "�̹� �����ϴ� ���� ������ �̸��� ���� ������ �� �����ϴ� : {:s}", sceneName);
			return nullptr;
		}

		UnloadScene();

		auto pNewScene = new Scene(sceneName);
		s_pActiveScene = pNewScene;
		return pNewScene;
	}

	Scene* SceneManager::LoadFromFile(const std::filesystem::path& path)
	{
		UnloadScene();

		auto pNewScene = new Scene;
		SceneSerializer serializer(pNewScene);
		if (!serializer.Deserialize(path))
		{
			DV_DELETE(pNewScene);
			return nullptr;
		}

		s_pActiveScene = pNewScene;
		return pNewScene;
	}

	void SceneManager::SaveToFile(const std::filesystem::path& path)
	{
		SceneSerializer serializer(s_pActiveScene);
		serializer.Serialize(path);
	}

	void SceneManager::UnloadScene()
	{
		if (s_pActiveScene)
		{
			DV_DELETE(s_pActiveScene);
		}
	}
}