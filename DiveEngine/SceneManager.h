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
		// GetSceneByPath

		unsigned int GetLoadedSceneCount() const;

	private:
		SceneManager();
		SceneManager(const SceneManager&) = delete;
		SceneManager operator=(const SceneManager&) = delete;

	private:
		// loaded scene count => �ᱹ �ϳ� �̻��� �ε� �Ѵٴ� �ǹ̴�.
		// �׸��� GetSceneAt()�� �����ϸ� �̵��� list�� �����ϴ� �� ����.
		// �ϴ��� vector�� ����.
		std::vector<Scene*> m_LoadedScene;

		// scene count in build settings

		// ���� �����ϴ°� ��������...? �ƴϸ� m_LoadedScene���� Scene���� ������ bool ���� �־�� �Ѵ�...
		Scene* m_ActiveScene = nullptr;

		// �ᱹ scene folder path�� �ʿ��ϴ�.
		// �׸��� �ش� path���� name�� ��� ���� �� �־�� �Ѵ�.
		// �׷��� ����Ƽ�� ��� �̸� scene�� ������...
	};
}