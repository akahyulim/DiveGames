#include "divepch.h"
#include "Renderable.h"
#include "Resource/ResourceManager.h"
#include "Renderer/Graphics/Texture2D.h"

void Dive::Renderable::CopyAttributes(Component* pCloneCom)
{
	if (!pCloneCom)
		return;

	auto pClone = dynamic_cast<Renderable*>(pCloneCom);
	if (!pClone)
		return;

	pClone->m_pMaterial = m_pMaterial;
}

void Dive::Renderable::SetMaterial(Material* pMaterial)
{
	m_pMaterial = pMaterial;

	ResourceManager::GetInstance().Cache<Material>(pMaterial);
}

void Dive::Renderable::SetMaterial(const std::string& filepath)
{
	// 파일에서 로드
}

std::string Dive::Renderable::GetMaterialName() const
{
	return m_pMaterial ? m_pMaterial->GetName() : "";
}

void Dive::Renderable::UseDefaultMaterial()
{
	auto defaultMaterial = new Material;
	
	const auto* defaultTex = ResourceManager::GetInstance().Load<Texture2D>("../Assets/Textures/no_texture.png");
	defaultMaterial->SetMap(eMaterialMapType::Albedo, defaultTex);

	SetMaterial(defaultMaterial);
}
