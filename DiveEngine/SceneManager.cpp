#include "SceneManager.h"
#include "Scene.h"

namespace dive
{
	Scene* SceneManager::CreateScene(const std::string& sceneName)
	{
		// scene 폴더에서 동일한 이름이 이미 존재하는지 확인해야 한다.
		// 아니면 loaded scene에서 동일한 이름이 존재하는지 확인한다.

		// 추후 변경 필요
		auto newScene = m_LoadedScene.emplace_back(new Scene);
		newScene->SetName(sceneName);

		return newScene;
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

	Scene* SceneManager::LoadSceneByName(const std::string& sceneName)
	{
		// load라는게 파일로 부터 scene을 만들라는 이야기인가...?
		// 그런데 예제에서는 Scene을 교체하는데 사용하는 것 같던데...?

		return nullptr;
	}
}