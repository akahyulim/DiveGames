#include "divepch.h"
#include "Material.h"
#include "Helper/YamlHelper.h"
#include "Graphics/Texture2D.h"
#include "Resource/ResourceManager.h"

// 1. 파일을 로드하면 각각의 Material이 만들어진다.
// 2. Material이 없다면 Default_Material을 사용한다.
//		- 수정이 불가능하다.
//		- no_texture가 사용된다.
//		- 유니티의 경우 오브젝트에 텍스쳐를 드래그할 경우 개별 Material이 생성된다. 이름은 해당 텍스처를 따라간다.
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

	// 일단 개별 파일로 저장한다.
	bool Material::SaveToFile(const std::string& filepath)
	{
		YAML::Emitter out;

		const auto albedoMap = HasMap(eMaterialMapType::Albedo) ? GetMap(eMaterialMapType::Albedo)->GetName() : "no_texture";

		out << YAML::BeginMap;
		out << YAML::Value << "m_Name" << YAML::Value << GetName();
		out << YAML::Value << "m_InstanceID" << YAML::Value << GetInstanceID();
		out << YAML::Value << "m_bEditable" << YAML::Value << IsEditable();
		out << YAML::Value << "m_AlbedoMap" << YAML::Value << albedoMap;
		out << YAML::Value << "m_AlbedoColor" << YAML::Value << GetAlbedoColor();
		out << YAML::Value << "m_Tiling" << YAML::Value << GetTiling();
		out << YAML::Value << "m_Offset" << YAML::Value << GetOffset();
		out << YAML::EndMap;

		// 임시
		auto path = "Assets/Materials/" + GetName() + ".mtrl";

		std::ofstream fout(path);
		fout << out.c_str();
		
		return true;
	}
	
	bool Material::LoadFromFile(const std::string& filepath)
	{
		auto nodes = YAML::LoadAllFromFile(filepath);
		if (nodes.empty())
		{
			DV_CORE_WARN("{:}가 존재하지 않습니다.", filepath);
			return false;
		}

		//m_Name;
		//m_InstanceID;
		//m_bEditable;
		//m_AlbedoMap;
		//m_AlbeodoColor;
		//m_Tiling;
		//m_Offset;

		return true;
	}
}