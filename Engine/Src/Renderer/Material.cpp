#include "divepch.h"
#include "Material.h"
#include "Helper/YamlHelper.h"
#include "Graphics/Texture.h"

namespace Dive
{
	Material::Material()
	{
		m_MainMaps[eMaterialMapType::Albedo] = nullptr;
		m_MainMaps[eMaterialMapType::Metalic] = nullptr;
		m_MainMaps[eMaterialMapType::Normal] = nullptr;
		m_MainMaps[eMaterialMapType::Occlusion] = nullptr;
		m_MainMaps[eMaterialMapType::Edmision] = nullptr;
		m_MainMaps[eMaterialMapType::DetailMask] = nullptr;
	}

	Material::Material(unsigned long long id)
		: Resource(id)
	{
		m_MainMaps[eMaterialMapType::Albedo] = nullptr;
		m_MainMaps[eMaterialMapType::Metalic] = nullptr;
		m_MainMaps[eMaterialMapType::Normal] = nullptr;
		m_MainMaps[eMaterialMapType::Occlusion] = nullptr;
		m_MainMaps[eMaterialMapType::Edmision] = nullptr;
		m_MainMaps[eMaterialMapType::DetailMask] = nullptr;
	}

	Material::~Material()
	{
	}

	Texture2D* Material::GetMap(const eMaterialMapType type)
	{
		return m_MainMaps[type];
	}

	void Material::SetMap(const eMaterialMapType type, const Texture2D* pTexture)
	{
		m_MainMaps[type] = const_cast<Texture2D*>(pTexture);
	}

	// 현재 구현대로라면 개별 파일로 저장된다.
	// 하지만 실제로는 Model별로 Material이 하나의 파일에 묶여야 한다.
	// 심지어 이름이 같은 mtrl도 있다...
	bool Material::SaveToFile(const std::string& filepath)
	{
		// 현재 이름이 없다.
		/*
		YAML::Emitter out;

		auto albedoMap = HasMap(eMaterialMapType::Albedo) ? GetMap(eMaterialMapType::Albedo)->GetName() : "";

		out << YAML::BeginMap;
		out << YAML::Value << "m_Name"			<< YAML::Value << GetName();
		out << YAML::Value << "m_InstanceID"	<< YAML::Value << GetInstanceID();
		out << YAML::Value << "m_AlbedoMap"		<< YAML::Value << albedoMap;
		out << YAML::Value << "m_AlbedoColor"	<< YAML::Value << GetAlbedoColor();
		out << YAML::Value << "m_Tiling"		<< YAML::Value << GetTiling();
		out << YAML::Value << "m_Offset"		<< YAML::Value << GetOffset();
		out << YAML::EndMap;

		// 임시
		auto path = "Assets/Materials/" + GetName() + ".mtrl";

		std::ofstream fout(path);
		fout << out.c_str();
		*/
		return true;
	}
	
	bool Material::LoadFromFile(const std::string& filepath)
	{
		return false;
	}
}