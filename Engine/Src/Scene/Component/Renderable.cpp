#include "divepch.h"
#include "Renderable.h"
#include "Resource/ResourceManager.h"

void Dive::Renderable::SetMaterial(Material* pMaterial)
{
	m_pMaterial = pMaterial;

	ResourceManager::GetInstance().Cache<Material>(pMaterial);
}
