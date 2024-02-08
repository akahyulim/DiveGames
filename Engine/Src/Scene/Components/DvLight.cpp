#include "DivePch.h"
#include "DvLight.h"
#include "DvTransform.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	DvLight::DvLight(DvGameObject* pGameObject)
		: DvComponent(pGameObject)
		, m_Type(eLightType::Directional)
		, m_Dir(0.0f, -1.0f, 0.0f)
		, m_Color(1.0f, 1.0f, 1.0f)
		, m_Range(10.0f)
	{
	}

	DvLight::~DvLight()
	{
	}
}