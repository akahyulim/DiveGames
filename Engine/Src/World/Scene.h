#pragma once

namespace Dive
{
	class GameObject;
	class Component;

	// 언리얼에선 기본적으로 Game과 Editor 두 개의 타입으로 구분된다.
	// 그리고 CreateWorld라는 Static 메서드를 제공하며, 내부에서 GEngine을 호출해 Engine에 저장된다.
	// 파일화의 경우 월드를 구성하는 대부분의 리소스(모델, 텍스쳐 등)들고 함께 패키징된다.

	// 일단은 Resource를 상속시키지 않았다.
	// 추후 상속을 생각한다면 일관성 유지를 위해 
	// ResourceManager를 없애고 Resource 타입별 매니저를 만들어 사용해야 한다.
	class Scene
	{
	public:
		Scene();
		Scene(int index, const std::string& name = "Untitled");	// 언리얼에선 굳이 해시로 관리하지 않는다. 그런데 이름때문에 guid가 필요하다..
		~Scene();

		void Clear();

		void Update();

		bool LoadFromFile(const std::string& fileName);
		bool SaveToFile(const std::string& fileName);
		
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
		bool IsLoaded() const { return m_bLoaded; }

		const std::string GetName() const { return m_Name; }
		void Rename(const std::string& newName) { m_Name = newName; }

		const std::string GetPath() const { return m_Path; }

		int GetIndex() const { return m_Index; }

	private:
		//Scene();
		//~Scene();

		uint64_t getFreeGameObjectID();
	
	private:
		static uint32_t s_WorldCounter;

		std::string m_Name;
		std::string m_Path;

		std::unordered_map<uint64_t, GameObject*> m_GameObjects;

		uint64_t m_CurGameObjectID;

		bool m_bDirty;

		bool m_bLoaded;

		int m_Index;
	};
}