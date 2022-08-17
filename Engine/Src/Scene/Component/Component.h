#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Scene;
	class GameObject;

	class Component : public Object
	{
		DIVE_OBJECT(Component, Object)

		friend class Scene;
		friend class GameObject;

	public:
		Component(Context* pContext);
		virtual ~Component() = default;

		GameObject* GetGameObject() { return m_pGameObject; }
		unsigned int GetID() const { return m_ID; }

	protected:
		void setGameObject(GameObject* pGameObject);
		void setID(unsigned int id);

	protected:
		GameObject* m_pGameObject;
		// id�� gameobject�� �����ϸ� gameobject ������ �������� ������.
		// ���� scene�� ������ �־�� �Ѵ�.
		unsigned int m_ID;
	};
}