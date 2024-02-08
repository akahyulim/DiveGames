#pragma once

namespace Dive
{
	class DvGameObject;
	class DvComponent;

	class DvScene
	{
	public:
		DvScene(const std::string& name = "Untitled");
		~DvScene();

		void Clear();
		
		DvGameObject* CreateGameObject(const std::string& name = "DvGameObject");
		void RemoveGameObject(DvGameObject* pGameObject);
		void RemoveGameObjectByID(uint64_t id);
		DvGameObject* GetGameObjectByID(uint64_t id);
		bool ExistsGameObject(DvGameObject* pGameObject);
		bool ExistsGameObjectByID(uint64_t id);
		std::vector<DvGameObject*> GetRootGameObjects();
		std::vector<DvGameObject*> GetAllGameObjects();
		uint64_t GetGameObjectsCount();
		 bool IsEmpty() const { return m_GameObjects.empty(); }

		void RegisterComponent(DvComponent* pComponent, uint64_t id = 0);
		void DeregisterComponent(DvComponent* pComponent);
		void DeregisterComponentByID(uint64_t id);
		DvComponent* GetComponent(uint64_t id);

		 std::string GetName() { return m_Name; }

	private:
		uint64_t getFreeGameObjectID();
		uint64_t getFreeComponentID();
	
	private:
		std::string m_Name;

		std::unordered_map<uint64_t, DvGameObject*> m_GameObjects;
		std::unordered_map<uint64_t, DvComponent*> m_Components;

		uint64_t m_CurGameObjectID;
		uint64_t m_CurComponentID;

		bool m_bDirty;
	};
}