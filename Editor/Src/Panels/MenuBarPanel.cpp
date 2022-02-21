#include "MenuBarPanel.h"
#include "ScenePanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "AssetPanel.h"

MenuBarPanel::MenuBarPanel(Editor* pEditor)
	: Panel(pEditor, "MenuBar")
{
	m_bWindow = false;
}

MenuBarPanel::~MenuBarPanel()
{
	DV_DELETE(m_pActiveScene);
}

// 메뉴바는 항상 그려져야 한다.
void MenuBarPanel::renderAlways()
{
	ImGui::BeginMainMenuBar();

	menuFile();
	menuGameObject();
	menuComponent();
	menuWindow();
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
				if (m_pActiveScene)
					DV_DELETE(m_pActiveScene);

				m_pActiveScene = new Dive::Scene("Sample_World");
				m_pActiveScene->CreateGameObject("Choa");
				m_pActiveScene->CreateGameObject("IU");
				m_pActiveScene->CreateGameObject("Knave");
				m_pActiveScene->CreateGameObject(13254521934309852950, "Complex");

				// 전달까지 직접하는게 맞다.
				// 하지만 방법이 너무 막무가내다....
				// ActiveScene을 static 포인터로 하면 어떨까...?
				m_pEditor->GetScene()->SetActiveScene(m_pActiveScene);
				m_pEditor->GetHierarchy()->SetActiveScene(m_pActiveScene);
				m_pEditor->GetInspector()->SetActiveScene(m_pActiveScene);
			}
			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Close"))
		{
			if (m_pActiveScene)
				DV_DELETE(m_pActiveScene);

			m_pEditor->GetScene()->SetActiveScene(m_pActiveScene);
			m_pEditor->GetHierarchy()->SetActiveScene(m_pActiveScene);
			m_pEditor->GetInspector()->SetActiveScene(m_pActiveScene);
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
		if (ImGui::MenuItem("Create Empty"))
		{
			if (m_pActiveScene)
			{
				m_pActiveScene->CreateGameObject("GameObject");
			}
		}

		if (ImGui::BeginMenu("3D Object"))
		{
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Light"))
		{
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Camera"))
		{
			ImGui::EndMenu();
		}

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

void MenuBarPanel::menuWindow()
{
	if (ImGui::BeginMenu("Window"))
	{
		// 해당 Panel의 객체를 가져와야 한다.

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