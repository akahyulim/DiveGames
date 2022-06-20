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

	void Light::CopyAttributes(Component* pCloneCom)
	{
		if (!pCloneCom)
			return;

		auto clone = dynamic_cast<Light*>(pCloneCom);
		if (!clone)
			return;

		clone->m_LightType = this->m_LightType;
		clone->m_bEnabled = this->m_bEnabled;
		clone->m_Color = this->m_Color;
		clone->m_Range = this->m_Range;
		clone->m_Intensity = this->m_Intensity;
		clone->m_SpotAngle = this->m_SpotAngle;
	}
}