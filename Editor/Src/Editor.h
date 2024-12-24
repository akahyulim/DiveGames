#pragma once
#include "Dive.h"
#include "Panels/Panel.h"

namespace Dive
{
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
		Menubar,
		WorldView,
		GameView,
		HierarchyView,
		InspectorView,
		AssetView,
		Max
	};

	struct ProjectSettings
	{
		std::string name;
		std::string path;
		std::string materialsPath;
		std::string modelsPath;
		std::string texturesPath;
		std::string worldsPath;
	};

	class Editor
	{
	public:
		Editor();
		~Editor() = default;

		int Run();

		void OnPostRender();

		Panel& GetPanel(ePanelTypes type);
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
		void initializeImGui();
		void renderImGui();

	private:
		std::array<std::unique_ptr<Panel>, static_cast<size_t>(ePanelTypes::Max)> m_Panels;
		std::array<ImFont*, static_cast<size_t>(eFontTypes::Max)> m_Fonts;

		ProjectSettings m_ProjectSettings;
		bool m_bProjectLoaded;
	};
}