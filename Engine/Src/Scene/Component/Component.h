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
		// id를 gameobject가 관리하면 gameobject 단위로 나뉘어져 버린다.
		// 따라서 scene가 관리해 주어야 한다.
		unsigned int m_ID;
	};
}