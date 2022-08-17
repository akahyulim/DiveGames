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
		// ���� ������Ʈ.
		void Update(float delta);

		// ���ο� ���� ������Ʈ�� ����.
		GameObject* CreateGameObject(const std::string& name = std::string(), unsigned int id = 0);
		// ���� ��ϵ� ���� ������Ʈ�� ����.
		void RemoveGameObject(GameObject* pGameObject);
		// ���� ��ϵ� ���� ������Ʈ�� ����.
		void RemoveGameObject(unsigned int id);
		// ���� ��ϵ� ���� ������Ʈ�� ID�� ȹ��.
		GameObject* GetGameObject(unsigned int id);
		// ���� ��ϵ� ���� ������Ʈ�� ���� ����.
		unsigned int GetGameObjectsCount() const { return static_cast<unsigned int>(m_GameObjects.size()); }
		// ���� ��ϵ� ���� ������Ʈ�� �� ��Ʈ ������ ���ͷ� �����Ͽ� ����.
		// �̷��� �ϱ� ���� ��ü GameObject�� ������ ����
		// Editor���� ��ġ�� ������ �� �ְԲ� �ϴ� ���� ���� ���̶� ����������,
		// �ᱹ ���� �����̴�... ���� const�� ���� ������ �����ϴ� ���� ���� ������ �ʹ�.
		// �׷��� ����, Hierarchy â���� ��� ���̿� �ִ� ����� �𸥴�.
		std::vector<GameObject*> GetRoots() const;
		
		// ���� ������Ʈ�� �߰��� ������Ʈ�� ����.
		void ComponentAdded(Component* pComponent, unsigned int id);
		// ���� ������Ʈ�� ���ŵ� ������Ʈ�� ����.
		void ComponentRemoved(Component* pComponent);
		// ������Ʈ�� ID�� �������� ������Ʈ �����͸� ã�� ����.
		Component* GetComponent(unsigned int id);

		// Serialization

		// �� �̸� ����. 
		std::string GetName() const { return m_Name; }
		// �� �̸� ����.
		void SetName(const std::string& name) { m_Name = name; }

		// ������Ʈ ���� ���� ����.
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
		// Component���� �� ������ �Բ� �����ϴ� ���� ���� �� �ִ�.
	};
}