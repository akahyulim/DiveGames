#pragma once
#include "Core/Object.h"

namespace Dive 
{
	class Context;
	class GameObject;
	class Component;
	class Event;

	static const unsigned int FIRST_ID = 0x1;
	static const unsigned int LAST_ID = 0xffffffff;

	// GameObject의 관리가 주 임무 + 추후 Octree를 추가할 수 있다.
	// 구성된 Scene을 Serialization 할 수 있어야 한다.
	// View가 GameObject와 Component를 참조하여 그려질 대상을 분류한다.
	class Scene : public Object
	{
		DIVE_OBJECT(Scene, Object)
		
	public:
		explicit Scene(Context* pContext);
		~Scene();

		// 씬을 구성하는 모든 게임오브젝트를 제거.
		void Clear();
		// 씬을 업데이트.
		void Update(float delta);

		// 새로운 게임 오브젝트를 생성.
		GameObject* CreateGameObject(const std::string& name = std::string(), unsigned int id = 0);
		// 씬에 등록된 게임 오브젝트를 제거.
		void RemoveGameObject(GameObject* pGameObject);
		// 씬에 등록된 게임 오브젝트를 제거.
		void RemoveGameObject(unsigned int id);
		// 씬에 등록된 게임 오브젝트를 ID로 획득.
		GameObject* GetGameObject(unsigned int id);
		// 씬에 등록된 게임 오브젝트의 개수 리턴.
		unsigned int GetGameObjectsCount() const { return static_cast<unsigned int>(m_GameObjects.size()); }
		// 씬에 등록된 게임 오브젝트들 중 루트 노드들을 벡터로 구성하여 리턴.
		// 이렇게 하기 보단 전체 GameObject를 전달한 다음
		// Editor에서 위치도 변경할 수 있게끔 하는 것이 나을 것이라 생각했지만,
		// 결국 같은 벡터이니... 차라리 const를 떼고 참조로 전달하는 것이 낫지 않을까 싶다.
		// 그런데 현재, Hierarchy 창에서 요소 사이에 넣는 방법을 모른다.
		std::vector<GameObject*> GetRoots() const;
		
		// 게임 오브젝트에 추가된 컴포넌트를 저장.
		void ComponentAdded(Component* pComponent, unsigned int id);
		// 게임 오브젝트에 제거된 컴포넌트를 삭제.
		void ComponentRemoved(Component* pComponent);
		// 컴포넌트의 ID로 관리중인 컴포넌트 포인터를 찾아 리턴.
		Component* GetComponent(unsigned int id);

		// Serialization

		// 씬 이름 리턴. 
		std::string GetName() const { return m_Name; }
		// 씬 이름 설정.
		void SetName(const std::string& name) { m_Name = name; }

		// 업데이트 적용 유무 설정.
		void SetUpdateEnabled(bool bEnable);

		// update(+ OnUpdate)
		void OnUpdate(const Event& e);

	private:
		unsigned int getFreeGameObjectID();
		unsigned int getFreeComponentID();

	private:
		std::string m_Name;
		bool m_bUpdateEnabled;
		bool m_bDirty;

		std::unordered_map<unsigned int, GameObject*> m_GameObjects;
		std::unordered_map<unsigned int, Component*> m_Components;

		unsigned int m_GameObjectID;
		unsigned int m_ComponentID;
		// Component까지 이 곳에서 함께 관리하는 편이 나을 수 있다.
	};
}