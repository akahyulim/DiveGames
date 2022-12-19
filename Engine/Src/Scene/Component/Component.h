#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Scene;
	class GameObject;

	// ������Ʈ ���̽� Ŭ����.
	class Component : public Object
	{
		DIVE_OBJECT(Component, Object)

		friend class Scene;
		friend class GameObject;

	public:
		explicit Component(Context* pContext);
		virtual ~Component() = default;

		GameObject* GetGameObject() { return m_pGameObject; }
		uint32_t GetID() const { return m_ID; }

	protected:
		void setGameObject(GameObject* pGameObject);
		void setID(uint32_t id);

	protected:
		GameObject* m_pGameObject;
		uint32_t m_ID;
	};
}