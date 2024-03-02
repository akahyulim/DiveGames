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
		DV_CORE_TRACE("������Ʈ({0:s}'s {1:s}) �Ҹ�", GetName(), GetTypeName());
	}
}