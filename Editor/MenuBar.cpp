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
					// 검색해보니 Menu에선 modal popup이 안된다는 이슈가 있다.
					// 해결책을 찾아보자.
					if (dive::Scene::GetGlobalScene().IsDirty())
						ImGui::OpenPopup("Delete?");
					
					// Always center this window when appearing
					ImVec2 center = ImGui::GetMainViewport()->GetCenter();
					ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

					// 아래에서 클리어 하기 전에 떠야하므로 위치는 맞다.
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
						// 팝업
						// 기존 Scene에 변경 사항이 존재합니다. 저장하시겠습니까?
					}

					// 디렉토리 탐색기를 연다.
					{
						std::string filepath = "../Assets/Scenes/default.scene";
						dive::Scene::GetGlobalScene().LoadFromFile(filepath);
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save"))
				{
					// 동일한 이름의 파일이 있는지 확인한다.
					// 만약 있다면 덮어씌울 것인지 물어야 한다.
					{
						std::string filepath = "../Assets/Scenes/default.scene";
						dive::Scene::GetGlobalScene().SaveToFile(filepath);

						APP_TRACE("현재 Scene과 Asset을 파일로 저장하였습니다.");
					}
				}

				if (ImGui::MenuItem("Save As..."))
				{
					// Scene 폴더를 연 후 이름을 입력 받아 저장한다.
					// Scene의 이름을 변경한 후
					// 저장할 폴더 경로를 SaveToFile에 넣으면 된다.
				}

				ImGui::Separator();

				// 프로젝트는 게임 전체를 의미한다.
				// 일단 뒤로 미루자.
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
					// 일단은 Scene의 변경사항 여부만 확인한다.
					if (dive::Scene::GetGlobalScene().IsDirty())
					{

					}
					// 하지만 추후 Project의 변경사항까지 확인해야 한다.

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

		// flag 때문에 자꾸 전체 윈도우 크기가 바뀐다.
		// 현재 창이 뜨지도 않는다.
		if (ImGui::BeginPopupModal("SaveDirty", NULL, 0))//ImGuiWindowFlags_AlwaysAutoResize))
		{
			// 팝업
			// 기존 Scene에 변경 사항이 존재합니다. 저장하시겠습니까?
			// 저장, 저장 안함, 취소
			ImGui::Text("Save?");

			ImGui::EndPopup();
		}
	}
}