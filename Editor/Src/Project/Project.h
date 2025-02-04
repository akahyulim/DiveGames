#pragma once
#include "dive.h"
#include <filesystem>

namespace Dive
{
	struct ProjectConfig
	{
		std::filesystem::path Materials;
		std::filesystem::path Models;
		std::filesystem::path Textures;
		std::filesystem::path Worlds;

		std::string OpenedWorld;
	};

	class Project
	{
	public:
		const std::filesystem::path& GetFilePath() const { return m_FilePath; }
		std::wstring GetNameW() const { return m_FilePath.stem().wstring(); }
		std::string GetName() const { return m_FilePath.stem().string(); }

		const ProjectConfig& GetConfig() const { return m_Config; }
		const std::filesystem::path& GetMaterialsPath() const { return m_Config.Materials; }
		const std::filesystem::path& GetModelsPath() const { return m_Config.Models; }
		const std::filesystem::path& GetTexturesPath() const { return m_Config.Textures; }
		const std::filesystem::path& GetWorldsPath() const { return m_Config.Worlds; }
		
		const std::string& GetOpenedWorldName() const { return m_Config.OpenedWorld; }
		bool SetOpenedWorldName(const std::string& name);

		static Project* New(const std::filesystem::path& filePath);
		static bool SaveToFile(Project* pProject);
		static Project* LoadFromFile(const std::filesystem::path& filePath);

	private:
		std::filesystem::path m_FilePath;
		ProjectConfig m_Config;
	};
}