#include "DivePch.h"
#include "Light.h"
#include "Core/CoreDefs.h"
#include "Scene/GameObject.h"

namespace Dive
{
	Light::Light(GameObject* pGameObject)
		: Component(pGameObject)
		, m_Type(eLightType::Directional)
		, m_Color(1.0f, 1.0f, 1.0f)
		, m_Range(10.0f)
		, m_OuterAngle(0.0f)
		, m_InnerAngle(0.0f)
		, m_bEnabled(true)
	{
	}

	Light::~Light()
	{
		DV_CORE_TRACE("ÄÄÆ÷³ÍÆ®({0:s}'s {1:s}) ¼Ò¸ê", GetName(), GetTypeName());
	}
}