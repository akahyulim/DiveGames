#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;

	class Camera : public Component
	{
	public:
		Camera(GameObject* pGameObject);
		~Camera() = default;

	private:
	private:
	};
}