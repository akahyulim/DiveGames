#pragma once
#include "dive.h"
#include <filesystem>

namespace Dive
{
	struct ProjectConfig
	{
		std::filesystem::path MaterialDir;
		std::filesystem::path ModelDir;
		std::filesystem::path TextureDir;
		std::filesystem::path WorldDir;

		std::string OpenedWorld;

		std::vector<std::filesystem::path> Worlds;
	};

	class Project
	{
	public:
		const std::filesystem::path& GetFilePath() const { return m_FilePath; }
		std::wstring GetNameW() const { return m_FilePath.stem().wstring(); }
		std::string GetName() const { return m_FilePath.stem().string(); }

		const ProjectConfig& GetConfig() const { return m_Config; }
		const std::filesystem::path& GetMaterialsPath() const { return m_Config.MaterialDir; }
		const std::filesystem::path& GetModelsPath() const { return m_Config.ModelDir; }
		const std::filesystem::path& GetTexturesPath() const { return m_Config.TextureDir; }
		const std::filesystem::path& GetWorldsPath() const { return m_Config.WorldDir; }
		
		const std::string& GetOpenedWorldName() const { return m_Config.OpenedWorld; }
		bool SetOpenedWorldName(const std::string& name);

		static Project* New(const std::filesystem::path& filePath);
		static bool SaveToFile(Project* project);
		static Project* LoadFromFile(const std::filesystem::path& filePath);

	private:
		std::filesystem::path m_FilePath;
		ProjectConfig m_Config;
	};
}
