#pragma once
#include "dive.h"
#include <filesystem>

namespace Dive
{
	struct ProjectConfig
	{
		std::string Name = "Untitled";
		std::filesystem::path AssetDirectory;
		std::filesystem::path StartWorld;
	};

	class Project
	{
	public:
		Project() : m_bModified(false) {}
		~Project() = default;
 
		const ProjectConfig& GetConfig() { return m_Config; }
		void SetConfig(const ProjectConfig& config);

		bool IsModified() const { return m_bModified; }

		static std::shared_ptr<Project> New();
		static std::shared_ptr<Project> Load(const std::filesystem::path& path);
		static bool Save(const std::filesystem::path& path);

		static std::shared_ptr<Project> GetActive() { return s_pActiveProject; }

		static const std::filesystem::path& GetDirectory()
		{
			DV_ASSERT(Project, s_pActiveProject);
			return s_pActiveProject->m_ProjectDirectory;
		}

		static const std::filesystem::path& GetAssetDirectory()
		{
			DV_ASSERT(Project, s_pActiveProject);
			return GetDirectory() / s_pActiveProject->m_Config.AssetDirectory;
		}

		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)
		{
			DV_ASSERT(Project, s_pActiveProject);
			return GetAssetDirectory() / path;
		}

	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;

		bool m_bModified;

		static std::shared_ptr<Project> s_pActiveProject;
	};
}