#pragma once
#include "Dive.h"
#include "Panels/Panel.h"

namespace Dive
{
	class Project;

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
		ProjectView,
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

		void NewWorld();
		void OpenWorld();
		void SaveWorld();
		void SaveWorldAs();
		void NewProject();
		void OpenProject();
		void SaveProject();
		void Exit();

		World* GetActiveWorld() { return m_pActiveWorld; }
		const std::filesystem::path GetActiveWorldFilePath() const;
		const std::filesystem::path GetProjectDir() const;

	private:
		void updateTitle();

		void drawMainManubar();
		void showPopups();

		void activateSaveChanges(std::function<void()> confirmCallback);

	private:
		std::array<ImFont*, static_cast<size_t>(eFontTypes::Max)> m_Fonts;
		std::vector<std::unique_ptr<Panel>> m_Panels;

		Project* m_pActiveProject;
		World* m_pActiveWorld;
		std::shared_ptr<GameObject> m_pSelectedGameObject;

		bool m_bProjectChanged;
		bool m_bWorldChanged;

		bool m_bShowSaveChangesPopup;
		bool m_bShowNewProjectPopup;
		bool m_bShowNewWorldPopup;

		std::function<void()> m_OnConfirm;
	};
}