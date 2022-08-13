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
		void Update(float delta);

		// 새로운 게임 오브젝트를 생성.
		GameObject* CreateGameObject(const std::string& name = std::string(), unsigned int id = 0);
		// 씬에 등록된 게임 오브젝트를 제거.
		void RemoveGameObject(GameObject* pGameObject);
		// 씬에 등록된 게임 오브젝트를 제거.
		void RemoveGameObject(unsigned int id);
		// 씬에 등록된 게임 오브젝트를 ID로 획득.
		GameObject* GetGameObject(unsigned int id);
		
		

		// Serialization

		// 업데이트 적용 유무 설정.
		void SetUpdateEnabled(bool bEnable);

		// update(+ OnUpdate)
		void OnUpdate(const Event& e);

	private:
		unsigned int getFreeID();

	private:
		bool m_bUpdateEnabled;

		std::unordered_map<unsigned int, GameObject*> m_GameObjects;

		unsigned int m_GameObjectID;

		// Component까지 이 곳에서 함께 관리하는 편이 나을 수 있다.
	};
}