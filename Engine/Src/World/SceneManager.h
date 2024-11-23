#pragma once

namespace Dive
{
	class Scene;

	class SceneManager
	{
	public:
		SceneManager(const SceneManager&) = delete;
		void operator=(const SceneManager&) = delete;

		static SceneManager* GetInstance()
		{
			if (!s_pInstance)
				s_pInstance = new SceneManager;

			return s_pInstance;
		}

		void Shutdown();

		Scene* CreateScene(const std::string& sceneName);

		void LoadScene(const std::string& sceneName);
		void LoadScene(int index);
		// 비동기 로드

		bool UnloadScene(const std::string& sceneName);
		bool UnloadScene(int index);

		Scene* GetActiveScene();
		bool SetActiveScene(Scene* pScene);

		Scene* GetSceneAt(int index);
		Scene* GetSceneByName(const std::string& sceneName);
		Scene* GetSceneByPath(const std::string& scenePath);

		uint32_t GetSceneCount();
	
	private:
		SceneManager();
		~SceneManager();

	private:
		static SceneManager* s_pInstance;

		std::vector<Scene*> m_Scenes;
		Scene* m_pActiveScene;
	};

	SceneManager* GetSceneManager();
}