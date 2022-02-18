#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;

	class Transform : public Component
	{
	public:
		Transform(GameObject* pGameObject);
		~Transform() = default;

	private:
	private:
	};
}