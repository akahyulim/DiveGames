#pragma once
#include "Dive.h"
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

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

	class Editor
	{
	public:
		static Editor& GetInstance()
		{
			static Editor instance;
			return instance;
		}

		int Run();

		void OnPostRender();

		ImFont* GetFont(eFontTypes type);

		void NewScene();
		void OpenScene();
		void SaveScene();
		void SaveSceneAs();
		void NewProject();
		void OpenProject();
		void SaveProject();
		void Exit();

		Scene* GetActiveScene() { return m_pEditScene; }
		const std::filesystem::path GetActiveSceneFilePath() const;
		const std::filesystem::path GetProjectDir() const;

		GameObject* GetEditorCamera() const { return m_pEditorCamera; }

	private:
		Editor();
		~Editor();

		void drawMainManubar();
		void showPopups();

		void activateSaveChanges(std::function<void()> confirmCallback);

		void createEditorOnly();

	private:
		std::array<ImFont*, static_cast<size_t>(eFontTypes::Max)> m_Fonts;

		Project* m_pActiveProject;
		Scene* m_pEditScene;
		std::shared_ptr<GameObject> m_pSelectedGameObject;

		bool m_bProjectChanged;

		bool m_bShowSaveChangesPopup;
		bool m_bShowNewProjectPopup;
		bool m_bShowNewScenePopup;

		std::function<void()> m_OnConfirm;

		// editor game objects
		GameObject* m_pEditorCamera;
	};
}