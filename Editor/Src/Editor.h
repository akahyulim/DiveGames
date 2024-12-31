#pragma once
#include "Dive.h"
#include "Panels/Panel.h"

namespace Dive
{
	class Project;

	struct ProjectSettings
	{
		std::string name;
		std::string path;
		std::string materialsPath;
		std::string modelsPath;
		std::string texturesPath;
		std::string worldsPath;
	};
	
	enum class eFontTypes
	{
		Default,
		Normal,
		Bold,
		Large,
		Large_Bold,
		Max
	};

	enum class ePanelTypes
	{
		WorldView,
		GameView,
		HierarchyView,
		InspectorView,
		AssetView,
		Max
	};

	class Editor
	{
	public:
		Editor();
		~Editor();

		int Run();

		void OnPostRender();

		ImFont* GetFont(eFontTypes type);

		ProjectSettings& GetProjectSettings() { return m_ProjectSettings; }
		bool CreateProject(const std::filesystem::path& path);
		bool OpenProject(const std::filesystem::path& path);
		void CloseProject();

		std::string GetProjectName() const { return m_ProjectSettings.name; }
		std::string GetProjectPath() const { return m_ProjectSettings.path; }
		std::string GetProjectMaterialsPath() const { return m_ProjectSettings.materialsPath; }
		std::string GetProjectModelsPath() const { return m_ProjectSettings.modelsPath; }
		std::string GetProjectTexturesPath() const { return m_ProjectSettings.texturesPath; }
		std::string GetProjectWorldsPath() const { return m_ProjectSettings.worldsPath; }

		bool IsProjectLoaded() const { return m_bProjectLoaded; }

		void SetTitle(const std::string& projectName);

	private:
		void drawMenubar();

	private:
		std::array<ImFont*, static_cast<size_t>(eFontTypes::Max)> m_Fonts;
		std::array<std::unique_ptr<Panel>, static_cast<size_t>(ePanelTypes::Max)> m_Panels;

		ProjectSettings m_ProjectSettings;
		bool m_bProjectLoaded;

		std::shared_ptr<Project> m_pActiveProject;
		std::shared_ptr<World> m_pActiveWorld;
	};
}