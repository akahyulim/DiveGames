#include "divepch.h"
#include "Renderable.h"
#include "Scene/DvGameObject.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	Renderable::Renderable(DvGameObject* pGameObject)
		: DvComponent(pGameObject)
		, m_pMaterial(nullptr)
		, m_pMesh(nullptr)
	{
	}

	Renderable::~Renderable()
	{
	}
}