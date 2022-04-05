#include "divepch.h"
#include "Material.h"

namespace Dive
{
	Material::Material()
	{
		m_MainMaps[eMaterialMapType::Albedo]		= nullptr;
		m_MainMaps[eMaterialMapType::Metalic]		= nullptr;
		m_MainMaps[eMaterialMapType::Normal]		= nullptr;
		m_MainMaps[eMaterialMapType::Occlusion]		= nullptr;
		m_MainMaps[eMaterialMapType::Edmision]		= nullptr;
		m_MainMaps[eMaterialMapType::DetailMask]	= nullptr;
	}

	Texture* Material::GetMap(const eMaterialMapType type)
	{
		return m_MainMaps[type];
	}

	void Material::SetMap(const eMaterialMapType type, const Texture* pTexture)
	{
		m_MainMaps[type] = const_cast<Texture*>(pTexture);
	}
}