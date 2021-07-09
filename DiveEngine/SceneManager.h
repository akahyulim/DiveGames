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
		// GetSceneByPath

		unsigned int GetLoadedSceneCount() const;

	private:
		SceneManager();
		SceneManager(const SceneManager&) = delete;
		SceneManager operator=(const SceneManager&) = delete;

	private:
		// loaded scene count => 결국 하나 이상을 로드 한다는 의미다.
		// 그리고 GetSceneAt()을 참고하면 이들을 list로 관리하는 것 같다.
		// 일단은 vector로 하자.
		std::vector<Scene*> m_LoadedScene;

		// scene count in build settings

		// 직접 관리하는게 나으려나...? 아니면 m_LoadedScene에서 Scene에게 일종의 bool 값을 주어야 한다...
		Scene* m_ActiveScene = nullptr;

		// 결국 scene folder path가 필요하다.
		// 그리고 해당 path에서 name을 모두 읽을 수 있어야 한다.
		// 그런데 유니티의 경우 이를 scene이 가진다...
	};
}