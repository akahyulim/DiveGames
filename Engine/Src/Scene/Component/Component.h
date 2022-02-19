#pragma once
#include "Base/Base.h"

namespace Dive
{
	class GameObject;

	class Component
	{
	public:
		Component(GameObject* pGameObject) : m_pGameObject(pGameObject)
		{
			DV_ASSERT(pGameObject != nullptr);
		}
		virtual ~Component() {}

		GameObject* GetGameObject() { return m_pGameObject; }

	protected:
	protected:
		GameObject* m_pGameObject = nullptr;
	};
}