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
		Project* pNewProject = new Project;
		pNewProject->m_FilePath = filePath;
		pNewProject->m_Config.Materials = filePath.parent_path() / "Assets" / "Materials";
		pNewProject->m_Config.Models = filePath.parent_path() / "Assets" / "Models";
		pNewProject->m_Config.Textures = filePath.parent_path() / "Assets" / "Textures";
		pNewProject->m_Config.Worlds = filePath.parent_path() / "Assets" / "Worlds";

		try
		{
			std::filesystem::create_directories(pNewProject->m_Config.Materials);
			std::filesystem::create_directories(pNewProject->m_Config.Models);
			std::filesystem::create_directories(pNewProject->m_Config.Textures);
			std::filesystem::create_directories(pNewProject->m_Config.Worlds);
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			DV_LOG(Project, err, "������Ʈ ���丮 ������ �����Ͽ����ϴ� : {:s}", e.what());
			DV_DELETE(pNewProject);
		}

		// ���� �� ����ȭ
		if (!Project::SaveToFile(pNewProject))
		{
			std::filesystem::remove_all(filePath.parent_path());
			DV_DELETE(pNewProject);
		}

		return pNewProject;
	}

	bool Project::SaveToFile(Project* pProject)
	{
		if (!pProject || pProject->m_FilePath.empty())
		{
			DV_LOG(Project, warn, "����ȭ�� �� �ִ� ��ü�� �������� �ʽ��ϴ�.");
			return false;
		}

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Materials" << YAML::Value << pProject->m_Config.Materials.string();
			out << YAML::Key << "Models" << YAML::Value << pProject->m_Config.Models.string();
			out << YAML::Key << "Textures" << YAML::Value << pProject->m_Config.Textures.string();
			out << YAML::Key << "Worlds" << YAML::Value << pProject->m_Config.Worlds.string();
			out << YAML::Key << "OpenedWorld" << YAML::Value << pProject->m_Config.OpenedWorld;
		}
		out << YAML::EndMap;

		if (!out.good())
		{
			DV_LOG(Project, err, "YAML ����ȭ ���� ������ �߻��Ͽ����ϴ�.");
			return false;
		}

		std::ofstream fout(pProject->m_FilePath);
		if (!fout)
		{
			DV_LOG(Project, err, "����({})�� �� �� �����ϴ�.", pProject->m_FilePath.string());
			return false;
		}
		fout << out.c_str();

		DV_LOG(Project, info, "������Ʈ({}) ����ȭ ����", pProject->GetName());

		return true;
	}

	Project*  Project::LoadFromFile(const std::filesystem::path& filePath)
	{
		Project* pLoadedProject = new Project;
		pLoadedProject->m_FilePath = filePath;

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filePath.string());
		}
		catch (YAML::BadFile& e)
		{
			DV_LOG(Project, warn, "������Ʈ ����({}) �ε忡 �����Ͽ����ϴ�. : {}", filePath, e.what());
			DV_DELETE(pLoadedProject);
			return nullptr;
		}
		catch (YAML::ParserException& e)
		{
			DV_LOG(Project, warn, "YAML ����({}) �Ľ̿� �����Ͽ����ϴ�. : {}", filePath, e.what());
			DV_DELETE(pLoadedProject);
			return nullptr;
		}

		auto projectNode = data["Project"];
		if (!projectNode)
		{
			DV_LOG(Project, warn, "������Ʈ ��带 ã�� �� �����ϴ�.");
			DV_DELETE(pLoadedProject);
			return nullptr;
		}

		pLoadedProject->m_Config.Materials = projectNode["Materials"].as<std::string>();
		pLoadedProject->m_Config.Models = projectNode["Models"].as<std::string>();
		pLoadedProject->m_Config.Textures = projectNode["Textures"].as<std::string>();
		pLoadedProject->m_Config.Worlds = projectNode["Worlds"].as<std::string>();
		pLoadedProject->m_Config.OpenedWorld = projectNode["OpenedWorld"].as<std::string>();

		DV_LOG(Project, info, "������Ʈ({}) ������ȭ ����", pLoadedProject->GetName());

		return pLoadedProject;
	}
}