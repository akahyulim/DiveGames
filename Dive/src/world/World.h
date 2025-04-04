#pragma once
#include "core/CoreDefs.h"

namespace Dive
{
	class GameObject;

	enum class eRenderableType
	{
		Unkonw,
		Camera,
		StaticModel,
		SkinnedModel,
	};

	// 유니티에서의 인터페이스를 보면
	// 굳이 World와 Editor를 구분할 필요가 없었다.
	// dirty는 물론 filePath까지 직접 관리하는 편이 나아보인다.
	class World
	{
	public:
		World(const std::string& name = "NewWorld");
		~World();

		void Clear();

		void Update();

		GameObject* CreateGameObject(const std::string& name = "GameObject");
		GameObject* CreateGameObject(UINT64 id, const std::string& name = "GameObject");
		void DeleteGameObject(GameObject* gameObject);
		void DeleteGameObjectByID(UINT64 id);
		GameObject* GetGameObjectByID(UINT64 id);
		bool ExistsGameObject(GameObject* gameObject);
		bool ExistsGameObjectByID(UINT64 id);
		std::vector<GameObject*> GetRootGameObjects();
		std::vector<GameObject*> GetAllGameObjects();
		UINT64 GetGameObjectsCount();

		bool IsEmpty() const { return m_GameObjects.empty(); }

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name);

		std::filesystem::path GetFilePath() const { return m_FilePath; }

		bool IsDirty() const { return m_IsDirty; }

		void OnModified();

	private:
		std::string m_Name;
		std::filesystem::path m_FilePath;

		UINT64 m_CurGameObjectID;
		std::unordered_map<UINT64, GameObject*> m_GameObjects;
		std::unordered_map<eRenderableType, std::vector<GameObject*>> m_Renderables;

		bool m_IsDirty;

		friend class WorldSerializer;

		// 이하 유니티 인터페이스
		// 런타임 데이타 구조체라고 한다.
		// buildIndex, isDirty, isLoaded, name, path, rootCount
		// GetRootGameObjects, IsValid
		// GameObject는 생성시 자동으로 활성화된 World에 추가된다. GameObject에 scene이라는 멤버 변수도 있다.
		// 언제부터였는지 모르겠지만 WorldManager란 것이 있다.
		
		// => 현재 내가 구현한 World는 언리얼의 구조에 가깝다.
	};
}
