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

		// 자신이 소속된 GameObject의 IsntanceID를 리턴
		unsigned long long GetInstanceID() const;

	protected:
		GameObject* m_pGameObject = nullptr;
	};
}