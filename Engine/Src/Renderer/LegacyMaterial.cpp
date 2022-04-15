#include "divepch.h"
#include "LegacyMaterial.h"
#include "Graphics/Texture.h"

namespace Dive
{
	LegacyMaterial::LegacyMaterial()
	{
		m_MainMaps[eMaterialMapType::Albedo]		= nullptr;
		m_MainMaps[eMaterialMapType::Metalic]		= nullptr;
		m_MainMaps[eMaterialMapType::Normal]		= nullptr;
		m_MainMaps[eMaterialMapType::Occlusion]		= nullptr;
		m_MainMaps[eMaterialMapType::Edmision]		= nullptr;
		m_MainMaps[eMaterialMapType::DetailMask]	= nullptr;
	}

	LegacyMaterial::~LegacyMaterial()
	{
	}

	Texture2D* LegacyMaterial::GetMap(const eMaterialMapType type)
	{
		return m_MainMaps[type];
	}

	void LegacyMaterial::SetMap(const eMaterialMapType type, const Texture2D* pTexture)
	{
		m_MainMaps[type] = const_cast<Texture2D*>(pTexture);
	}
}