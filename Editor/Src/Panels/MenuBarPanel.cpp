#include "MenuBarPanel.h"
#include "imgui-docking/imgui.h"
#include "Base/Editor.h"
#include "DiveEngine.h"
#include <Windows.h>

namespace Dive
{
	MenuBarPanel::MenuBarPanel(Editor* pEditor)
		: m_pEditor(pEditor)
	{
		DV_ASSERT(m_pEditor);
	}

	void MenuBarPanel::RenderPanel()
	{
		ImGui::BeginMainMenuBar();

		menuFile();
		menuGameObject();
		menuComponent();
		menuHelp();

		ImGui::EndMainMenuBar();
	}

	void MenuBarPanel::menuFile()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::BeginMenu("New"))
			{
				if (ImGui::MenuItem("Project"))
				{

				}
				if (ImGui::MenuItem("Scene"))
				{

				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Open"))
			{
				if (ImGui::MenuItem("Project"))
				{

				}
				if (ImGui::MenuItem("Scene"))
				{

				}
				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Close"))
			{

			}
			if (ImGui::MenuItem("Close Project"))
			{

			}

			ImGui::Separator();

			if (ImGui::MenuItem("Save"))
			{

			}
			if (ImGui::MenuItem("Save As..."))
			{

			}
			if (ImGui::MenuItem("Save Project"))
			{

			}

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
			{
				// Scene 저장 확인

				m_pEditor->Close();
			}

			ImGui::EndMenu();
		}
	}

	void MenuBarPanel::menuGameObject()
	{
		if (ImGui::BeginMenu("GameObject"))
		{
			ImGui::EndMenu();
		}
	}

	void MenuBarPanel::menuComponent()
	{
		if (ImGui::BeginMenu("Component"))
		{
			ImGui::EndMenu();
		}
	}

	void MenuBarPanel::menuHelp()
	{
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::EndMenu();
		}
	}
}