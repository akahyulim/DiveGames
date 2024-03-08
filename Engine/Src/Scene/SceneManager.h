#pragma once

namespace Dive
{
	class Scene;

	class SceneManager
	{
	public:

		static void Shutdown();

		static Scene* CreateScene(const std::string& sceneName);

		static void LoadScene(const std::string& sceneName);
		static void LoadScene(int index);
		// 비동기 로드

		static bool UnloadScene(const std::string& sceneName);
		static bool UnloadScene(int index);

		static Scene* GetActiveScene();
		static bool SetActiveScene(Scene* pScene);

		static Scene* GetSceneAt(int index);
		static Scene* GetSceneByName(const std::string& sceneName);
		static Scene* GetSceneByPath(const std::string& scenePath);

		static uint32_t GetSceneCount();
	};
}