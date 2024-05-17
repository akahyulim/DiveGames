#pragma once
#include "Core/Object.h"

namespace Dive
{
	class GameObject;

	class Component : public Object
	{
		DV_CLASS(Component, Object);

	public:
		Component(GameObject* pGameObject);
		~Component() override = default;

		virtual void Update() {}

		std::string GetName() const;

		GameObject* GetGameObject() const;
		
	protected:
		GameObject* m_pGameObject;
	};
}