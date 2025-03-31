#include "Project.h"
#include "core/Engine.h"
#include <yaml-cpp/yaml.h>
#include <fstream>

namespace Dive
{
	bool Project::SetOpenedWorldName(const std::string& name)
	{
		if (m_Config.OpenedWorld == name)
			return false;

		m_Config.OpenedWorld = name;

		return true;
	}

	Project* Project::New(const std::filesystem::path& filePath)
	{
		auto project = new Project;
		project->m_FilePath = filePath;
		project->m_Config.MaterialDir = filePath.parent_path() / "Assets" / "Materials";
		project->m_Config.ModelDir = filePath.parent_path() / "Assets" / "Models";
		project->m_Config.TextureDir = filePath.parent_path() / "Assets" / "Textures";
		project->m_Config.WorldDir = filePath.parent_path() / "Assets" / "Worlds";

		try
		{
			std::filesystem::create_directories(project->m_Config.MaterialDir);
			std::filesystem::create_directories(project->m_Config.ModelDir);
			std::filesystem::create_directories(project->m_Config.TextureDir);
			std::filesystem::create_directories(project->m_Config.WorldDir);
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			DV_LOG(Project, err, "������Ʈ ���丮 ������ �����Ͽ����ϴ� : {:s}", e.what());
			DV_DELETE(project);
		}

		// ���� �� ����ȭ
		if (!Project::SaveToFile(project))
		{
			std::filesystem::remove_all(filePath.parent_path());
			DV_DELETE(project);
		}

		return project;
	}

	bool Project::SaveToFile(Project* project)
	{
		if (!project || project->m_FilePath.empty())
		{
			DV_LOG(Project, warn, "����ȭ�� �� �ִ� ��ü�� �������� �ʽ��ϴ�.");
			return false;
		}

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "MaterialDir" << YAML::Value << project->m_Config.MaterialDir.string();
			out << YAML::Key << "ModelDir" << YAML::Value << project->m_Config.ModelDir.string();
			out << YAML::Key << "TextureDir" << YAML::Value << project->m_Config.TextureDir.string();
			out << YAML::Key << "WorldDir" << YAML::Value << project->m_Config.WorldDir.string();
			out << YAML::Key << "OpenedWorld" << YAML::Value << project->m_Config.OpenedWorld;
		}
		out << YAML::EndMap;

		if (!out.good())
		{
			DV_LOG(Project, err, "YAML ����ȭ ���� ������ �߻��Ͽ����ϴ�.");
			return false;
		}

		std::ofstream fout(project->m_FilePath);
		if (!fout)
		{
			DV_LOG(Project, err, "����({})�� �� �� �����ϴ�.", project->m_FilePath.string());
			return false;
		}
		fout << out.c_str();

		DV_LOG(Project, info, "������Ʈ({}) ����ȭ ����", project->GetName());

		return true;
	}

	Project*  Project::LoadFromFile(const std::filesystem::path& filePath)
	{
		auto project = new Project;
		project->m_FilePath = filePath;

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filePath.string());
		}
		catch (YAML::BadFile& e)
		{
			DV_LOG(Project, warn, "������Ʈ ����({}) �ε忡 �����Ͽ����ϴ�. : {}", filePath, e.what());
			DV_DELETE(project);
			return nullptr;
		}
		catch (YAML::ParserException& e)
		{
			DV_LOG(Project, warn, "YAML ����({}) �Ľ̿� �����Ͽ����ϴ�. : {}", filePath, e.what());
			DV_DELETE(project);
			return nullptr;
		}

		auto projectNode = data["Project"];
		if (!projectNode)
		{
			DV_LOG(Project, warn, "������Ʈ ��带 ã�� �� �����ϴ�.");
			DV_DELETE(project);
			return nullptr;
		}

		project->m_Config.MaterialDir = projectNode["MaterialDir"].as<std::string>();
		project->m_Config.ModelDir = projectNode["ModelDir"].as<std::string>();
		project->m_Config.TextureDir = projectNode["TextureDir"].as<std::string>();
		project->m_Config.WorldDir = projectNode["WorldDir"].as<std::string>();
		project->m_Config.OpenedWorld = projectNode["OpenedWorld"].as<std::string>();

		DV_LOG(Project, info, "������Ʈ({}) ������ȭ ����", project->GetName());

		return project;
	}
}