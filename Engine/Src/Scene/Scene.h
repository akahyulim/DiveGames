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

	// GameObject�� ������ �� �ӹ� + ���� Octree�� �߰��� �� �ִ�.
	// ������ Scene�� Serialization �� �� �־�� �Ѵ�.
	// View�� GameObject�� Component�� �����Ͽ� �׷��� ����� �з��Ѵ�.
	class Scene : public Object
	{
		DIVE_OBJECT(Scene, Object)
		
	public:
		explicit Scene(Context* pContext);
		~Scene();

		// ���� �����ϴ� ��� ���ӿ�����Ʈ�� ����.
		void Clear();
		void Update(float delta);

		// ���ο� ���� ������Ʈ�� ����.
		GameObject* CreateGameObject(const std::string& name = std::string(), unsigned int id = 0);
		// ���� ��ϵ� ���� ������Ʈ�� ����.
		void RemoveGameObject(GameObject* pGameObject);
		// ���� ��ϵ� ���� ������Ʈ�� ����.
		void RemoveGameObject(unsigned int id);
		// ���� ��ϵ� ���� ������Ʈ�� ID�� ȹ��.
		GameObject* GetGameObject(unsigned int id);
		
		

		// Serialization

		// ������Ʈ ���� ���� ����.
		void SetUpdateEnabled(bool bEnable);

		// update(+ OnUpdate)
		void OnUpdate(const Event& e);

	private:
		unsigned int getFreeID();

	private:
		bool m_bUpdateEnabled;

		std::unordered_map<unsigned int, GameObject*> m_GameObjects;

		unsigned int m_GameObjectID;

		// Component���� �� ������ �Բ� �����ϴ� ���� ���� �� �ִ�.
	};
}