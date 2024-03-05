#pragma once
#include "Core/Object.h"

namespace Dive
{
	class GameObject;
	class Transform;

	class Component : public DvObject
	{
		DV_OBJECT(Component, DvObject);

	public:
		Component(GameObject* pGameObject);
		~Component() override = default;

		virtual void Update() {}

		std::string GetName() const;

		GameObject* GetGameObject() const;
		Transform* GetTransform();

	protected:
		GameObject* m_pGameObject;
		Transform* m_pTransform;
	};
}