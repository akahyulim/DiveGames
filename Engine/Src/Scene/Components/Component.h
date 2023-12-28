#pragma once
#include "Core/Object.h"

namespace Dive
{
	class GameObject;
	class Transform;

	class Component : public Object
	{
	public:
		Component(GameObject* pGameObject);
		virtual ~Component() = default;

		virtual void Update() {}

		std::string GetName() const override;
		void SetName(const std::string&) override {}

		GameObject* GetGameObject() const;
		Transform* GetTransform();

	protected:
		GameObject* m_pGameObject;
		Transform* m_pTransform;
	};
}