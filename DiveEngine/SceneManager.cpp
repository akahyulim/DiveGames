#include "SceneManager.h"
#include "Scene.h"

namespace dive
{
	Scene* SceneManager::CreateScene(const std::string& sceneName)
	{
		// scene �������� ������ �̸��� �̹� �����ϴ��� Ȯ���ؾ� �Ѵ�.
		// �ƴϸ� loaded scene���� ������ �̸��� �����ϴ��� Ȯ���Ѵ�.

		// ���� ���� �ʿ�
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
		// load��°� ���Ϸ� ���� scene�� ������ �̾߱��ΰ�...?
		// �׷��� ���������� Scene�� ��ü�ϴµ� ����ϴ� �� ������...?

		return nullptr;
	}
}