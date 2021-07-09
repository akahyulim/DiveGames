#include "MenuBar.h"
#include "Editor.h"
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_internal.h"
#include <Windows.h>

namespace editor
{
	MenuBar::MenuBar(Editor* editor)
		: Widget(editor)
	{
		mTitle = "MenuBar";
		mbWindow = false;
		m_Scene = &dive::Scene::GetGlobalScene();
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
					// �˻��غ��� Menu���� modal popup�� �ȵȴٴ� �̽��� �ִ�.
					// �ذ�å�� ã�ƺ���.
					if (dive::Scene::GetGlobalScene().IsDirty())
						ImGui::OpenPopup("Delete?");
					
					// Always center this window when appearing
					ImVec2 center = ImGui::GetMainViewport()->GetCenter();
					ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

					// �Ʒ����� Ŭ���� �ϱ� ���� �����ϹǷ� ��ġ�� �´�.
					if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
						ImGui::Separator();

						//static int unused_i = 0;
						//ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

						static bool dont_ask_me_next_time = false;
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
						ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
						ImGui::PopStyleVar();

						if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
						ImGui::SetItemDefaultFocus();
						ImGui::SameLine();
						if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
						ImGui::EndPopup();
					}

					dive::Scene::GetGlobalScene().Clear();
				}

				if (ImGui::MenuItem("Open World"))
				{
					if (dive::Scene::GetGlobalScene().IsDirty())
					{
						// �˾�
						// ���� Scene�� ���� ������ �����մϴ�. �����Ͻðڽ��ϱ�?
					}

					// ���丮 Ž���⸦ ����.
					{
						std::string filepath = "../Assets/Scenes/default.scene";
						dive::Scene::GetGlobalScene().LoadFromFile(filepath);
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save"))
				{
					// ������ �̸��� ������ �ִ��� Ȯ���Ѵ�.
					// ���� �ִٸ� ����� ������ ����� �Ѵ�.
					{
						std::string filepath = "../Assets/Scenes/default.scene";
						dive::Scene::GetGlobalScene().SaveToFile(filepath);

						APP_TRACE("���� Scene�� Asset�� ���Ϸ� �����Ͽ����ϴ�.");
					}
				}

				if (ImGui::MenuItem("Save As..."))
				{
					// Scene ������ �� �� �̸��� �Է� �޾� �����Ѵ�.
					// Scene�� �̸��� ������ ��
					// ������ ���� ��θ� SaveToFile�� ������ �ȴ�.
				}

				ImGui::Separator();

				// ������Ʈ�� ���� ��ü�� �ǹ��Ѵ�.
				// �ϴ� �ڷ� �̷���.
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
					// �ϴ��� Scene�� ������� ���θ� Ȯ���Ѵ�.
					if (dive::Scene::GetGlobalScene().IsDirty())
					{

					}
					// ������ ���� Project�� ������ױ��� Ȯ���ؾ� �Ѵ�.

					DestroyWindow(mEditor->GetWindowHandle());
				}

				ImGui::EndMenu();
			}

			ImGui::SameLine(0.0f, 30.0f);

			if (ImGui::BeginMenu("GameObject"))
			{
				if (ImGui::MenuItem("Create Empty"))
				{
					m_Scene->CreateGameObject();
				}

				if (ImGui::BeginMenu("3D Object"))
				{
					if (ImGui::MenuItem("Triangle"))
					{
					}

					if (ImGui::MenuItem("Quad"))
					{
						auto gameObject = m_Scene->CreateGameObject();
						auto meshRenderer = gameObject->AddComponent<dive::MeshRenderer>();
						meshRenderer->CreateDefaultMesh(dive::eDefaultMeshType::Quad);
						gameObject->SetName("Quad");
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Cube"))
					{
						auto gameObject = m_Scene->CreateGameObject();
						auto meshRenderer = gameObject->AddComponent<dive::MeshRenderer>();
						meshRenderer->CreateDefaultMesh(dive::eDefaultMeshType::Cube);
						gameObject->SetName("Cube");
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
				// �ܵ� ���� �Ұ�!!! SelectedObject�� �־�� �Ѵ�.
				// => ���ٸ� �Ʒ��� �޴� ��Ȱ��
				// �׸��� Transform�� �������� �ʴ´�.
				// �ߺ� ������ Ȯ���� �ʿ��ϴ�.
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

				ImGui::MenuItem(u8"����");
				*/
				ImGui::EndMenu();
			}

			ImGui::SameLine(0.0f, 30.0f);

			if (ImGui::BeginMenu("Help"))
			{
				//ImGui::MenuItem(u8"Dive Editor ����", nullptr, &m_bPopupAbout);

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
		ImGui::PopStyleVar();

		// flag ������ �ڲ� ��ü ������ ũ�Ⱑ �ٲ��.
		// ���� â�� ������ �ʴ´�.
		if (ImGui::BeginPopupModal("SaveDirty", NULL, 0))//ImGuiWindowFlags_AlwaysAutoResize))
		{
			// �˾�
			// ���� Scene�� ���� ������ �����մϴ�. �����Ͻðڽ��ϱ�?
			// ����, ���� ����, ���
			ImGui::Text("Save?");

			ImGui::EndPopup();
		}
	}
}