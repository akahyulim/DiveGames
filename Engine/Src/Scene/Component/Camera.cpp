#include "divepch.h"
#include "Camera.h"

namespace Dive
{
	Camera::Camera(GameObject* pGameObject)
		: Component(pGameObject)
	{
		m_Type = eComponentType::Camera;
	}
}