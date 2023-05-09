#pragma once
#include "Core/Object.h"

namespace Dive
{
	class GameObject;

	class Component : public Object
	{
	public:
		Component(GameObject* pGameObject);
		~Component() = default;

		virtual void Update(float delta) {}

		GameObject* GetGameObject() const { return m_pGameObject; }

	protected:
		GameObject* m_pGameObject;
	};
}