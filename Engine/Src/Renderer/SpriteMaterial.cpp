#include "divepch.h"
#include "SpriteMaterial.h"
#include "Graphics/Texture.h"
#include "Helper/YamlHelper.h"

namespace Dive
{
	SpriteMaterial::SpriteMaterial(const std::string& name, unsigned long long id)
		: Material(name, id)
	{
	}

	SpriteMaterial::~SpriteMaterial()
	{
	}

	bool SpriteMaterial::SaveToFile(const std::string& filepath)
	{
		YAML::Emitter out;

		unsigned long long handle = HasSprite() ? GetSprite()->GetInstanceID() : 0;

		out << YAML::BeginMap;
		out << YAML::Key << "Material";
		out << YAML::BeginMap;
		out << YAML::Key << "m_Name" << YAML::Value << GetName();
		out << YAML::Key << "m_InstanceID" << YAML::Value << GetInstanceID();
		out << YAML::Key << "m_Sprite" << YAML::Value << handle;
		out << YAML::Key << "m_Color" << YAML::Value << GetColor();
		out << YAML::Key << "m_bFlipX" << YAML::Value << m_bFlipX;
		out << YAML::Key << "m_bFlipY" << YAML::Value << m_bFlipY;
		out << YAML::EndMap;
		out << YAML::EndMap;

		auto path = "Assets/Materials/" + GetName() + "dv_material";

		std::ofstream fout(path);
		fout << out.c_str();

		return true;
	}
	
	bool SpriteMaterial::LoadFromFile(const std::string& filepath)
	{
		return false;
	}
}