#include "divepch.h"
#include "Light.h"

namespace Dive
{
	Light::Light(GameObject* pGameObject, unsigned long long id)
		: Component(pGameObject, id)
	{
	}

	Light::~Light()
	{
	}
}