#pragma once

namespace Dive
{
	class Scene;

	// 유니티에선 SceneLoaded, SceneUnloaded, ActiveSceneChanged 세 개의 이벤트도 다룬다.
	class SceneManager
	{
	public:
		static Scene* CreateScene(const std::string& sceneName = "NewScene");

		static Scene* LoadFromFile(const std::filesystem::path& path);
		static void SaveToFile(const std::filesystem::path& path);

		static void UnloadScene();

		static Scene* GetActiveScene() { return s_pActiveScene; }

	private:
		static Scene* s_pActiveScene;
	};
}