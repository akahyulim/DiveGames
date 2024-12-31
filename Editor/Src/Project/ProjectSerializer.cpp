#include "ProjectSerializer.h"
#include <yaml-cpp/yaml.h>
#include <fstream>

namespace Dive
{
	ProjectSerializer::ProjectSerializer(std::shared_ptr<Project> pProject)
		: m_pProject(pProject)
	{
	}

	bool ProjectSerializer::Serialize(const std::filesystem::path& filepath)
	{
		const ProjectConfig& config = m_pProject->GetConfig();

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value;
		{
			out << YAML::Key << "Name" << YAML::Value << config.Name;
			out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
			out << YAML::Key << "StartWorld" << YAML::Value << config.StartWorld.string();
			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}
	
	bool ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		ProjectConfig config;

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			DV_LOG(ProjectSerializer, err, "프로젝트 파일({0:s}) 로드에 실패하였습니다 : {1:s}", filepath.string(), e.what());
			return false;
		}

		auto projectNode = data["Project"];
		if (!projectNode)
			return false;

		config.Name = projectNode["Name"].as<std::string>();
		config.AssetDirectory = projectNode["AssetDirectory"].as<std::string>();
		config.StartWorld = projectNode["StartWorld"].as<std::string>();

		m_pProject->SetConfig(config);

		return true;
	}
}