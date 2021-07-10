#pragma once
#include <string>
#include <vector>

namespace dive
{
	class Runtime;
	class Scene;

	class SceneManager
	{
	public:
		~SceneManager();

		static SceneManager& GetInstance()
		{
			static SceneManager instance;
			return instance;
		}

		// ceratescene : 이미 존재하는 이름을 검색할 수 있어야 한다.
		Scene* CreateScene(const std::string& sceneName);
		
		Scene* LoadScene(const std::string& scenePath);
		// LoadScneeAsyn
		// UnloadSceneAsync
		// MergeScenes
		// MovegGmeobjectToScene
		Scene* GetActiveScene() { return m_ActiveScene; }
		bool SetActiveScene(Scene* scene);

		Scene* GetSceneByName(const std::string& sceneName);

		unsigned int GetLoadedSceneCount() const { return static_cast<unsigned int>(m_LoadedScene.size()); }

	private:
		SceneManager();
		SceneManager(const SceneManager&) = delete;
		SceneManager operator=(const SceneManager&) = delete;

	private:
		std::vector<Scene*> m_LoadedScene;
		// scene count in build settings

		// 직접 관리하는게 나으려나...? 아니면 m_LoadedScene에서 Scene에게 일종의 bool 값을 주어야 한다...
		Scene* m_ActiveScene = nullptr;
	};
}