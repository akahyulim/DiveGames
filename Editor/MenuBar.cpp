#include "MenuBar.h"
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_internal.h"
#include <Windows.h>

namespace Editor
{
	MenuBar::MenuBar(Editor* pEditor)
		: Widget(pEditor)
	{
		m_title = "MenuBar";
		m_bWindow = false;
		m_pScene = &Dive::Scene::GetGlobalScene();
	}

	void MenuBar::TickAlways()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(GetPadding(), GetPadding()));
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New World"))
				{
				}

				if (ImGui::MenuItem("Open World"))
				{
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save"))
				{
					// 기존 세이브가 없다면 지정된 경로에 scene이름으로 저장
					// 이후부턴 해당 scene 이름으로 새로 저장
				}

				if (ImGui::MenuItem("Save As..."))
				{
				}

				ImGui::Separator();

				if (ImGui::MenuItem("New Project"))
				{
				}

				if (ImGui::MenuItem("Open Project"))
				{
				}

				if (ImGui::MenuItem("Save Project"))
				{
				}

				// settings???

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					// 저장 여부를 확인해야 한다.

					PostQuitMessage(0);
				}

				ImGui::EndMenu();
			}

			ImGui::SameLine(0.0f, 30.0f);

			if (ImGui::BeginMenu("GameObject"))
			{
				if (ImGui::MenuItem("Create Empty"))
				{
					m_pScene->CreateGameObject();
				}

				if (ImGui::BeginMenu("3D Object"))
				{
					if (ImGui::MenuItem("Triangle"))
					{
					}

					if (ImGui::MenuItem("Quad"))
					{
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Cube"))
					{
					}

					if (ImGui::MenuItem("Plane"))
					{
					}

					if (ImGui::MenuItem("Sphere"))
					{
					}

					if (ImGui::MenuItem("Cylinder"))
					{
					}

					if (ImGui::MenuItem("Cone"))
					{
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Light"))
				{
					if (ImGui::MenuItem("Directional"))
					{
					}

					if (ImGui::MenuItem("Spot"))
					{
					}

					if (ImGui::MenuItem("Point"))
					{
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Camera"))
				{
				}

				ImGui::EndMenu();
			}

			ImGui::SameLine(0.0f, 30.0f);

			if (ImGui::BeginMenu("Component"))
			{
				// 단독 생성 불가!!! SelectedObject가 있어야 한다.
				// => 없다면 아래의 메뉴 비활성
				// 그리고 Transform은 포함하지 않는다.
				// 중복 생성도 확인이 필요하다.
				if (ImGui::MenuItem("Camera"))
				{
				}

				if (ImGui::MenuItem("Renderable"))
				{
				}

				if (ImGui::MenuItem("Light"))
				{
				}

				ImGui::EndMenu();
			}

			ImGui::SameLine(0.0f, 30.0f);

			if (ImGui::BeginMenu("Window"))
			{
				/*
				ImGui::MenuItem("World", NULL, &g_pWidgetScene->IsVisible());
				ImGui::MenuItem("Game", NULL, &g_pWidgetGame->IsVisible());
				ImGui::MenuItem("Hierarchy", NULL, &g_pWidgetHierarchy->IsVisible());
				ImGui::MenuItem("Components", NULL, &g_pWidgetInspector->IsVisible());
				ImGui::MenuItem("Log", NULL, &g_pWidgetConsole->IsVisible());
				ImGui::MenuItem("AssetsWidget", NULL, &g_pWidgetAssets->IsVisible());

				ImGui::Separator();

				ImGui::MenuItem("Image Importer");// , NULL, &g_pWidgetImageImporter->IsVisible());
				ImGui::MenuItem("Model Importer");

				ImGui::Separator();

				ImGui::MenuItem(u8"설정");
				*/
				ImGui::EndMenu();
			}

			ImGui::SameLine(0.0f, 30.0f);

			if (ImGui::BeginMenu("Help"))
			{
				//ImGui::MenuItem(u8"Dive Editor 정보", nullptr, &m_bPopupAbout);

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
		ImGui::PopStyleVar();
	}
}