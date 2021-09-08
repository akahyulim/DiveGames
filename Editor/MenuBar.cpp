#include "MenuBar.h"
#include "Editor.h"
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_internal.h"
#include <Windows.h>

namespace Editor
{
	MenuBar::MenuBar(Editor* pEditor)
		: Widget(pEditor)
	{
		m_Title = "MenuBar";
		m_bWindow = false;
		m_bModalNewScene = false;
	}

	// Scene�� ���� �� GameObject�� ������ �� ���µ� �׳� �õ��Ѵ�.
	void MenuBar::TickAlways()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(GetPadding(), GetPadding()));

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene"))
				{
					if (m_pScene)
					{
						if (m_pScene->IsDirty())
						{
							// �˾�â�� ��� Dirty�� �������� ������ ������ ���´�.
						}
					}

					// �˾��� ����� �ϴµ� ����� �ֳ� ����.
					m_bModalNewScene = true;
					//ImGui::OpenPopup("popup");
				}

				if (ImGui::MenuItem("Open Scene"))
				{
					//if (dive::Scene::GetGlobalScene().IsDirty())
					if (m_pScene)
					{
						if(m_pScene->IsDirty())
						{
						// �˾�
						// ���� Scene�� ���� ������ �����մϴ�. �����Ͻðڽ��ϱ�?
						}
					}

					// ���丮 Ž���⸦ ����.
					{
						std::string filepath = "../Assets/Scenes/MeshTestScene.scene";//"../Assets/Scenes/default.scene";
						dive::SceneManager::GetInstance().LoadScene(filepath);
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save"))
				{
					// ������ �̸��� ������ �ִ��� Ȯ���Ѵ�.
					// ���� �ִٸ� ����� ������ ����� �Ѵ�.
					{
						std::string filepath = "../Assets/Scenes/" + m_pScene->GetName() + ".scene";
						//dive::Scene::GetGlobalScene().SaveToFile(filepath);
						m_pScene->SaveToFile(filepath);

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
					//if (dive::Scene::GetGlobalScene().IsDirty())
					if(m_pScene->IsDirty())
					{

					}
					// ������ ���� Project�� ������ױ��� Ȯ���ؾ� �Ѵ�.

					DestroyWindow(m_pEditor->GetWindowHandle());
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
						auto pGameObject = m_pScene->CreateGameObject();
						auto meshRenderer = pGameObject->AddComponent<dive::MeshRenderer>();
						meshRenderer->CreateDefaultMesh(dive::eDefaultMeshType::Quad);
						pGameObject->SetName("Quad");
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Cube"))
					{
						auto pGameObject = m_pScene->CreateGameObject();
						auto meshRenderer = pGameObject->AddComponent<dive::MeshRenderer>();
						meshRenderer->CreateDefaultMesh(dive::eDefaultMeshType::Cube);
						pGameObject->SetName("Cube");
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
						auto pGameObject = m_pScene->CreateGameObject();
						auto pLight = pGameObject->AddComponent<dive::Light>();
						pLight->SetLightType(dive::eLightType::Directional);
						pGameObject->SetName("Directional Light");
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
					auto pGameObject = m_pScene->CreateGameObject();
					pGameObject->AddComponent<dive::Camera>();
					pGameObject->SetName("Camera");
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

		//= Popups ============================
		modalNewScene();

	}

	// ��ư�� ���� ���ĵǵ��� �ϰ� �ʹ�.
	void MenuBar::modalNewScene()
	{
		static char sceneName[16] = { 0, };
	
		if (m_bModalNewScene)
		{
			ImGui::OpenPopup("New Scene");
			m_bModalNewScene = false;
			ZeroMemory(sceneName, sizeof(sceneName));
		}

		// �̷��� �ϴ� �������� �ʴ´�.
		auto& io = ImGui::GetIO();
		//ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("New Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::NewLine();
			ImGui::Text("Input New Scene Name");
			ImGui::NewLine();

			ImGui::InputText("##edit", &sceneName[0], IM_ARRAYSIZE(sceneName));

			ImGui::NewLine();

			// �̷��� ���°� �ƴѰ�?
			ImGui::PushItemWidth(-100);

			if (ImGui::Button("Cancel", ImVec2(100, 30)))
			{
				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				return;
			}

			ImGui::SameLine(0.0f, 5.0f);

			if (ImGui::Button("Ok", ImVec2(100, 30)))
			{
				// �� ���ڿ��� �ȵȴ�.

				auto newScene = dive::SceneManager::GetInstance().CreateScene(sceneName);
				if(!newScene)
				{
					// �̰͵� m_b�� ������ �� �� ����.
					ImGui::OpenPopup("SameNameExists");

					if (ImGui::BeginPopupModal("SameNameExists", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::NewLine();
						ImGui::Text("Input New Scene Name");
						ImGui::NewLine();

						if (ImGui::Button("Ok"))
							m_bModalNewScene = true;

						ImGui::NewLine();

						ImGui::EndPopup();
					}

				}
				else
				{
					// �̰� ���� �ϴ°� ���� �� �׷���.
					dive::SceneManager::GetInstance().SetActiveScene(newScene);
				}

				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				return;
			}

			ImGui::NewLine();
		
			ImGui::EndPopup();
		}
	}
}