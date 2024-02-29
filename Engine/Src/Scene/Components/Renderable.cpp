#include "divepch.h"
#include "Renderable.h"
#include "Scene/GameObject.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	Renderable::Renderable(GameObject* pGameObject)
		: Component(pGameObject)
		, m_pMaterial(nullptr)
		, m_pMesh(nullptr)
	{
	}

	Renderable::~Renderable()
	{
		DV_CORE_TRACE("ÄÄÆ÷³ÍÆ®({0:s}'s {1:s}) ¼Ò¸ê", GetName(), GetTypeName());
	}
}