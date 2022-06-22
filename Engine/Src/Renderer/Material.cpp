#include "divepch.h"
#include "Material.h"
#include "Helper/YamlHelper.h"
#include "Graphics/Texture2D.h"
#include "Resource/ResourceManager.h"

// 1. ������ �ε��ϸ� ������ Material�� ���������.
// 2. Material�� ���ٸ� Default_Material�� ����Ѵ�.
//		- ������ �Ұ����ϴ�.
//		- no_texture�� ���ȴ�.
//		- ����Ƽ�� ��� ������Ʈ�� �ؽ��ĸ� �巡���� ��� ���� Material�� �����ȴ�. �̸��� �ش� �ؽ�ó�� ���󰣴�.
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

	// �ϴ� ���� ���Ϸ� �����Ѵ�.
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

		// �ӽ�
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
			DV_CORE_WARN("{:}�� �������� �ʽ��ϴ�.", filepath);
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