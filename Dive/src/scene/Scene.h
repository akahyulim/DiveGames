#pragma once

namespace Dive
{
	class GameObject;

	// 유니티에서의 인터페이스를 보면
	// 굳이 Scene와 Editor를 구분할 필요가 없었다.
	// dirty는 물론 filePath까지 직접 관리하는 편이 나아보인다.
	class Scene
	{
	public:
		Scene(const std::string& name = "NewScene");
		~Scene();

		void Clear();

		void Tick();

		GameObject* CreateGameObject(const std::string& name = "GameObject");
		void RemoveGameObject(GameObject* pGameObject);
		void RemoveGameObjectByID(uint64_t id);
		GameObject* GetGameObjectByID(uint64_t id);
		bool ExistsGameObject(GameObject* pGameObject);
		bool ExistsGameObjectByID(uint64_t id);
		std::vector<GameObject*> GetRootGameObjects();
		std::vector<GameObject*> GetAllGameObjects();
		uint64_t GetGameObjectsCount();

		bool IsEmpty() const { return m_GameObjects.empty(); }

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name);

		std::filesystem::path GetFilePath() const { return m_FilePath; }

		bool IsDirty() const { return m_bDirty; }

		void OnModified();
		
	private:
		uint64_t getFreeGameObjectID();

	private:
		std::string m_Name;
		std::filesystem::path m_FilePath;

		uint64_t m_CurGameObjectID;
		std::unordered_map<uint64_t, GameObject*> m_GameObjects;

		bool m_bDirty;

		friend class SceneSerializer;

		// 이하 유니티 인터페이스
		// 런타임 데이타 구조체라고 한다.
		// buildIndex, isDirty, isLoaded, name, path, rootCount
		// GetRootGameObjects, IsValid
		// GameObject는 생성시 자동으로 활성화된 Scene에 추가된다. GameObject에 scene이라는 멤버 변수도 있다.
		// 언제부터였는지 모르겠지만 SceneManager란 것이 있다.
		
		// => 현재 내가 구현한 Scene는 언리얼의 구조에 가깝다.
	};
}