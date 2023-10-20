#include "DivePch.h"
#include "Light.h"
#include "Transform.h"
#include "Core/Log.h"

namespace Dive
{
	Light::Light(GameObject* pGameObject)
		: Component(pGameObject),
		m_Type(eLightType::Directional),
		m_Dir(0.0f, -1.0f, 0.0f),
		m_Color(1.0f, 1.0f, 1.0f),
		m_Range(10.0f),
		m_bEnabled(true)
	{
	}

	Light::~Light()
	{
	}
}