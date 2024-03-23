#pragma once
#include "Core/Object.h"

namespace Dive
{
	class GameObject;

	class Component : public DvObject
	{
		DV_CLASS(Component, DvObject);

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