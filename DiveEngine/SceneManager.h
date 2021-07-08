#pragma once
#include <string>
#include <vector>

namespace dive
{
	class Scene;

	class SceneManager
	{
	public:

		// ceratescene : �̹� �����ϴ� �̸��� �˻��� �� �־�� �Ѵ�.
		Scene* CreateScene(const std::string& sceneName);
		Scene* GetActiveScene();
		Scene* GetSceneByName(const std::string& sceneName);
		// GetSceneByPath
		Scene* LoadSceneByName(const std::string& sceneName);	// path�ϼ��� �ִ�...
		// LoadScneeAsync
		// MergeScenes
		// MovegGmeobjectToScene
		// SetActiveScene
		// UnloadSceneAsync

		unsigned int GetLoadedSceneCount() const;

	private:
	private:
		// loaded scene count => �ᱹ �ϳ� �̻��� �ε� �Ѵٴ� �ǹ̴�.
		// �׸��� GetSceneAt()�� �����ϸ� �̵��� list�� �����ϴ� �� ����.
		// �ϴ��� vector�� ����.
		std::vector<Scene*> m_LoadedScene;

		// scene count in build settings

		// ���� �����ϴ°� ��������...? �ƴϸ� m_LoadedScene���� Scene���� ������ bool ���� �־�� �Ѵ�...
		Scene* m_ActiveScene;

		// �ᱹ scene folder path�� �ʿ��ϴ�.
		// �׸��� �ش� path���� name�� ��� ���� �� �־�� �Ѵ�.
		// �׷��� ����Ƽ�� ��� �̸� scene�� ������...
	};
}