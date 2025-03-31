#pragma once

namespace Dive
{
	class Scene;

	// ����Ƽ���� SceneLoaded, SceneUnloaded, ActiveSceneChanged �� ���� �̺�Ʈ�� �ٷ��.
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