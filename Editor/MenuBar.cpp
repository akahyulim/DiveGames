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
		m_Scene = nullptr;
		m_bPopupSceneName = false;

		EVENT_SUBSCRIBE(dive::eEventType::SceneActivate, EVENT_HANDLE(OnSetActiveScene));
	}

	void MenuBar::TickAlways()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(GetPadding(), GetPadding()));
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene"))
				{
					if (m_Scene)
					{
						if (m_Scene->IsDirty())
						{
							// 팝업창을 띄워 Dirty면 변경점을 저장할 것인지 묻는다.
						}
					}

					// 팝업을 띄워야 하는데 방법이 있나 보다.
					m_bPopupSceneName = true;
				}

				if (ImGui::MenuItem("Open Scene"))
				{
					if (dive::Scene::GetGlobalScene().IsDirty())
					{
						// 팝업
						// 기존 Scene에 변경 사항이 존재합니다. 저장하시겠습니까?
					}

					// 디렉토리 탐색기를 연다.
					{
						std::string filepath = "../Assets/Scenes/default.scene";
						dive::SceneManager::GetInstance().LoadScene(filepath);
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

		//= Popups ============================
		popupSetSceneName();
	}

	void MenuBar::OnSetActiveScene()
	{
		m_Scene = dive::SceneManager::GetInstance().GetActiveScene();
		assert(m_Scene);
	}

	// 화면 정중앙에 뜨도록 하고 싶다.
	// 버튼이 우측 정렬되도록 하고 싶다.
	void MenuBar::popupSetSceneName()
	{
		static char sceneName[32] = { 0, };
	
		if (m_bPopupSceneName)
		{
			ImGui::OpenPopup("##SetSceneName");
			m_bPopupSceneName = false;
			ZeroMemory(sceneName, sizeof(sceneName));
		}

		if (ImGui::BeginPopup("##SetSceneName"))
		{
			ImGui::Text("Input New Scene Name");
			ImGui::InputText("##edit", &sceneName[0], IM_ARRAYSIZE(sceneName));

			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				return;
			}

			ImGui::SameLine(0.0f, 5.0f);

			if (ImGui::Button("Ok"))
			{
				auto newScene = dive::SceneManager::GetInstance().CreateScene(sceneName);
				if(!newScene)
				{
					// 새 창을 띄워야 하나...
					// 여기에서 modaless를 띄우면 되지 않을까?
					// 그리고 그 창에서 다시 m_bPopupSceneName = true로 하고 말이지...
				}
				else
				{
					// 이걸 직접 하는게 아직 좀 그렇다.
					dive::SceneManager::GetInstance().SetActiveScene(newScene);
				}

				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				return;
			}

			ImGui::EndPopup();
		}
	}
}