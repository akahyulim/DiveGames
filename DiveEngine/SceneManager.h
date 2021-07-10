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

		// ceratescene : �̹� �����ϴ� �̸��� �˻��� �� �־�� �Ѵ�.
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

		// ���� �����ϴ°� ��������...? �ƴϸ� m_LoadedScene���� Scene���� ������ bool ���� �־�� �Ѵ�...
		Scene* m_ActiveScene = nullptr;
	};
}