#include "Project.h"
#include "ProjectSerializer.h"

namespace Dive
{
	std::shared_ptr<Project> Project::s_pActiveProject;
	
	void Project::SetConfig(const ProjectConfig& config)
	{
		auto updateConfig = [this](auto& member, const auto& newValue) {
			if (member != newValue) {
				member = newValue;
				m_bModified = true;
			}
		};

		updateConfig(m_Config.Name, config.Name);
		updateConfig(m_Config.AssetDirectory, config.AssetDirectory);
		updateConfig(m_Config.StartWorld, config.StartWorld);
	}

	std::shared_ptr<Project> Project::New()
	{
		s_pActiveProject = std::make_shared<Project>();
		s_pActiveProject->m_bModified = true;

		return s_pActiveProject;
	}
	
	std::shared_ptr<Project> Project::Load(const std::filesystem::path& path)
	{
		auto pProject = std::make_shared<Project>();

		ProjectSerializer serializer(pProject);
		if (serializer.Deserialize(path))
		{
			pProject->m_ProjectDirectory = path.parent_path();
			s_pActiveProject = pProject;
			s_pActiveProject->m_bModified = false;
			return s_pActiveProject;
		}

		return nullptr;
	}
	
	bool Project::Save(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(s_pActiveProject);
		if (serializer.Serialize(path))
		{
			s_pActiveProject->m_ProjectDirectory = path.parent_path();
			s_pActiveProject->m_bModified = false;
			return true;
		}

		return false;
	}
}