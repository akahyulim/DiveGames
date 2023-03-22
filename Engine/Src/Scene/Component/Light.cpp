#include "divepch.h"
#include "Light.h"
#include "IO/Log.h"

namespace Dive
{
	Light::Light(Context* pContext)
		: Component(pContext),
		m_Type(eLightType::Directional),
		m_Dir(0.0f, -1.0f, 0.0f),
		m_Color(1.0f, 1.0f, 1.0f),
		m_Range(10.0f),
		m_SpotAngle(60.0f),
		m_Intensity(10.0f),
		m_bEnabled(true)
	{
	}

	Light::~Light()
	{
	}
}