#pragma once
#include "Core/Object.h"

namespace Dive 
{
	class Context;
	class GameObject;
	class Component;
	class Event;

	static const uint32_t FIRST_ID = 0x1;
	static const uint32_t LAST_ID = 0xffffffff;

	// 게임 오브젝트를 관리하는 씬 클래스.
	// 현재 리소스를 상속하지 않고 있다.
	class Scene : public Object
	{
		DIVE_OBJECT(Scene, Object)
		
	public:
		explicit Scene(Context* pContext);
		~Scene();

		void Clear();
		
		void Update(float delta);

		GameObject* CreateGameObject(const std::string& name = std::string(), uint32_t id = 0);
		void RemoveGameObject(GameObject* pGameObject);
		void RemoveGameObject(uint32_t id);
		GameObject* GetGameObject(uint32_t id);
		uint32_t GetGameObjectsCount() const { return static_cast<uint32_t>(m_GameObjects.size()); }
		
		std::vector<GameObject*> GetRoots() const;
		std::vector<GameObject*> GetAllGameObjects() const;
		
		void RegisterComponent(Component* pComponent, uint32_t id);
		void DeregisterComponent(Component* pComponent);
		Component* GetComponent(uint32_t id);

		// Serialization

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		void SetUpdateEnabled(bool bEnable);
		bool IsUpdateEnabled() const { return m_bUpdateEnabled; }

		void OnUpdate(const Event& e);

	private:
		uint32_t getFreeGameObjectID();
		uint32_t getFreeComponentID();

	private:
		std::string m_Name;
		bool m_bUpdateEnabled;
		bool m_bDirty;

		std::unordered_map<uint32_t, GameObject*> m_GameObjects;
		std::unordered_map<uint32_t, Component*> m_Components;

		uint32_t m_CurGameObjectID;
		uint32_t m_CurComponentID;

		uint32_t m_OnUpdateSlotID;
	};
}