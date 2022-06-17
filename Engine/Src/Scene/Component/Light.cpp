#include "divepch.h"
#include "Light.h"

namespace Dive
{
	Light::Light(GameObject* pGameObject)
		: Component(pGameObject)
	{
	}

	Light::~Light()
	{
	}
}