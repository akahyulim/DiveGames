#include "MenuBarPanel.h"
#include "Editor.h"
#include "Geometry.h"
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_internal.h"
#include "External/ImGui/imgui_stdlib.h"
#include <Windows.h>
#include <Commdlg.h>
#include <ShlObj.h>
#include <atlstr.h>
#include <filesystem>

namespace Dive
{

	int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
	{
		switch (uMsg)
		{
		case BFFM_INITIALIZED:
		{
			::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);
			break;
		}

		default:
			break;
		}

		return 0;
	}


	MenuBarPanel::MenuBarPanel(Editor* pEditor)
		: Panel(pEditor)
	{
		m_Title				= "MenuBar";
		m_bWindow			= false;

		m_bModalNewProject	= false;
		m_bModalNewScene	= false;
	}

	// Scene�� ���� �� GameObject�� ������ �� ���µ� �׳� �õ��Ѵ�.
	void MenuBarPanel::TickAlways()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(GetPadding(), GetPadding()));

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("New"))
				{
					if (ImGui::MenuItem("Project"))
					{
						m_bModalNewProject = true;
					}

					if (ImGui::MenuItem("Scene"))
					{
						// ����â
						// Scene �̸� �Է�
						// ���, ����� ��ư
						// ������Ʈ�� ���Ե��� �ʾҴٸ� ��ε� ������ �����...
						// �ᱹ ���� Scene ������ �����ϰ� ������ �����ִ�.
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Open"))
				{
					if (ImGui::MenuItem("Project"))
					{
						// ������ Ư�� ������ ����� �Ѵ�.
						// 1 ������ ����� ���� ����
						// 2 ������ Editor�� ���� ����
						// �ϴ��� Editor�� ���� ������ ������ �Ѵ�.
						auto dir = std::filesystem::current_path().string() + '/';

						char targetFile[MAX_PATH] = "";

						OPENFILENAMEA ofn;
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize		= sizeof(OPENFILENAME);
						ofn.hwndOwner		= m_pEditor->GetWindowHandle();
						ofn.lpstrFilter		= "Project File\0*.proj\0";
						ofn.lpstrFile		= targetFile;
						ofn.nMaxFile		= MAX_PATH;
						ofn.lpstrInitialDir = dir.c_str();

						if (0 != GetOpenFileNameA(&ofn))
						{
							// ���õ� ���� Ȯ��
							APP_TRACE("Open Project: {:s}", targetFile);

							// ���� �ٸ� ������Ʈ�� ���� ���̰�
							// ���� ������ ������ ���
							// �޽����� �Բ� ����, �������, ��� ��ư�� �˾� â�� ���.
							
							// ���� ���õ� ������Ʈ ������ �ε��Ѵ�.

							// ������Ʈ ������ ���ڿ��� �����ϸ� �� ���� �� ����.
						}
					}

					if (ImGui::MenuItem("Scene"))
					{
						// ���� �켱 ������ �ٸ���.
						// 1 ������ ���� ������Ʈ ������ Assets\Scenes => ��, ������Ʈ ����� ������ �ʿ��ϴ�.
						// 2 ������ ���ó� Editor�� ��ġ ����
						auto dir = std::filesystem::current_path().string() + '/';

						char targetFile[MAX_PATH] = "";

						OPENFILENAMEA ofn;
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize		= sizeof(OPENFILENAME);
						ofn.hwndOwner		= m_pEditor->GetWindowHandle();
						ofn.lpstrFilter		= "Scene File\0*.scene\0";
						ofn.lpstrFile		= targetFile;
						ofn.nMaxFile		= MAX_PATH;
						ofn.lpstrInitialDir = dir.c_str();

						if (0 != GetOpenFileNameA(&ofn))
						{
							// ���õ� ���� Ȯ��
							APP_TRACE("Open Scene: {:s}", targetFile);

							// ���� �ٸ� ������Ʈ�� ���� ���̰�
							// ���� ������ ������ ���
							// �޽����� �Բ� ����, �������, ��� ��ư�� �˾� â�� ���.

							// ���� ���õ� ������Ʈ ������ �ε��Ѵ�.
						}

					}

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Close"))
				{
					// ���� scene ����
				}

				if (ImGui::MenuItem("Close Project"))
				{
					// ���� project ����
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save"))
				{
					// Scene �����̰���...
					// => VS�� ���� �����̴�.
				}

				if (ImGui::MenuItem("Save As..."))
				{
					// OPENFILENAME, GetSaveFileName 

					auto dir = std::filesystem::current_path().string() + '/';

					// ���� Scene �̸����� �����ؾ� �Ѵ�.
					char targetFile[MAX_PATH] = "";

					OPENFILENAMEA ofn;
					ZeroMemory(&ofn, sizeof(OPENFILENAME));
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = m_pEditor->GetWindowHandle();
					ofn.lpstrFilter = "Project File\0*.proj\0";
					ofn.lpstrFile = targetFile;
					ofn.nMaxFile = MAX_PATH;
					ofn.lpstrInitialDir = dir.c_str();

					if (0 != GetSaveFileNameA(&ofn))
					{
						// ���� ������ ������ �̸��� ������ �����ϴ��� Ȯ���ؾ� �Ѵ�.
						// �̸� ���� ��ü�� �� ���� �ؾ��ϳ� ����.
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					// ���� ����� ������ ���� ���
					// �޽����� �Բ� ����, �������, ��� ��ư�� �˾� â�� ���.
					// Scene �����̴�.

					// ����� ���ᰡ �ΰ�... �ƴѰ�...
					DestroyWindow(m_pEditor->GetWindowHandle());
				}

				ImGui::EndMenu();
			}

			ImGui::SameLine(0.0f, 30.0f);

			if (ImGui::BeginMenu("old File"))
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
					//if (Scene::GetGlobalScene().IsDirty())
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
						SceneManager::GetInstance().LoadScene(filepath);
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save"))
				{
					// ������ �̸��� ������ �ִ��� Ȯ���Ѵ�.
					// ���� �ִٸ� ����� ������ ����� �Ѵ�.
					{
						std::string filepath = "../Assets/Scenes/" + m_pScene->GetName() + ".scene";
						//Scene::GetGlobalScene().SaveToFile(filepath);
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
					//if (Scene::GetGlobalScene().IsDirty())
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
					if (ImGui::MenuItem("Cube"))
					{
						static StaticMesh* pMesh = nullptr;

						if (!pMesh)
						{
							std::vector<Dive::Vertex_StaticMesh> vertices;
							std::vector<uint32_t> indices;
							Utility::CreateCube(vertices, indices);

							pMesh = new Dive::StaticMesh();
							pMesh->SetVertices(vertices);
							pMesh->SetIndices(indices);
							pMesh->CreateBuffers(Renderer::GetInstance().GetGraphicsDevice()->GetDevice());
							pMesh->SetName("Cube");
						}

						auto pGameObject = m_pScene->CreateGameObject();
						auto pMeshRenderer = pGameObject->AddComponent<StaticMeshRenderer>();
						pMeshRenderer->SetMesh(pMesh);
						pGameObject->SetName(pMesh->GetName());
					}

					if (ImGui::MenuItem("Sphere"))
					{
						static StaticMesh* pMesh = nullptr;

						if (!pMesh)
						{
							std::vector<Dive::Vertex_StaticMesh> vertices;
							std::vector<uint32_t> indices;
							Utility::CreateSphere(vertices, indices);

							pMesh = new Dive::StaticMesh();
							pMesh->SetVertices(vertices);
							pMesh->SetIndices(indices);
							pMesh->CreateBuffers(Renderer::GetInstance().GetGraphicsDevice()->GetDevice());
							pMesh->SetName("Sphere");
						}

						auto pGameObject = m_pScene->CreateGameObject();
						auto pMeshRenderer = pGameObject->AddComponent<StaticMeshRenderer>();
						pMeshRenderer->SetMesh(pMesh);
						pGameObject->SetName(pMesh->GetName());
					}

					/*
					if (ImGui::MenuItem("Capsule"))
					{
					}
					*/

					if (ImGui::MenuItem("Cylinder"))
					{
						static StaticMesh* pMesh = nullptr;

						if (!pMesh)
						{
							std::vector<Dive::Vertex_StaticMesh> vertices;
							std::vector<uint32_t> indices;
							Utility::CreateCylinder(vertices, indices);

							pMesh = new Dive::StaticMesh();
							pMesh->SetVertices(vertices);
							pMesh->SetIndices(indices);
							pMesh->CreateBuffers(Renderer::GetInstance().GetGraphicsDevice()->GetDevice());
							pMesh->SetName("Cylinder");
						}

						auto pGameObject = m_pScene->CreateGameObject();
						auto pMeshRenderer = pGameObject->AddComponent<StaticMeshRenderer>();
						pMeshRenderer->SetMesh(pMesh);
						pGameObject->SetName(pMesh->GetName());
					}

					if (ImGui::MenuItem("Plane"))
					{
						static StaticMesh* pMesh = nullptr;

						if (!pMesh)
						{
							std::vector<Dive::Vertex_StaticMesh> vertices;
							std::vector<uint32_t> indices;
							Utility::CreatePlane(vertices, indices);

							pMesh = new Dive::StaticMesh();
							pMesh->SetVertices(vertices);
							pMesh->SetIndices(indices);
							pMesh->CreateBuffers(Renderer::GetInstance().GetGraphicsDevice()->GetDevice());
							pMesh->SetName("Plane");
						}

						auto pGameObject = m_pScene->CreateGameObject();
						auto pMeshRenderer = pGameObject->AddComponent<StaticMeshRenderer>();
						pMeshRenderer->SetMesh(pMesh);
						pGameObject->SetName(pMesh->GetName());
					}

					if (ImGui::MenuItem("Quad"))
					{
						static StaticMesh* pMesh = nullptr;

						if (!pMesh)
						{
							std::vector<Dive::Vertex_StaticMesh> vertices;
							std::vector<uint32_t> indices;
							Utility::CreateQuad(vertices, indices);

							pMesh = new Dive::StaticMesh();
							pMesh->SetVertices(vertices);
							pMesh->SetIndices(indices);
							pMesh->CreateBuffers(Renderer::GetInstance().GetGraphicsDevice()->GetDevice());
							pMesh->SetName("Quad");
						}

						auto pGameObject = m_pScene->CreateGameObject();
						auto pMeshRenderer = pGameObject->AddComponent<StaticMeshRenderer>();
						pMeshRenderer->SetMesh(pMesh);
						pGameObject->SetName(pMesh->GetName());
					}

					ImGui::Separator();

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Light"))
				{
					if (ImGui::MenuItem("Directional"))
					{
						auto pGameObject = m_pScene->CreateGameObject();
						auto pLight = pGameObject->AddComponent<Light>();
						pLight->SetLightType(eLightType::Directional);
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
					pGameObject->AddComponent<Camera>();
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
				ImGui::MenuItem("World", NULL, &g_pPanelScene->IsVisible());
				ImGui::MenuItem("Game", NULL, &g_pPanelGame->IsVisible());
				ImGui::MenuItem("Hierarchy", NULL, &g_pPanelHierarchy->IsVisible());
				ImGui::MenuItem("Components", NULL, &g_pPanelInspector->IsVisible());
				ImGui::MenuItem("Log", NULL, &g_pPanelConsole->IsVisible());
				ImGui::MenuItem("AssetsPanel", NULL, &g_pPanelAssets->IsVisible());

				ImGui::Separator();

				ImGui::MenuItem("Image Importer");// , NULL, &g_pPanelImageImporter->IsVisible());
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
		modalNewProject();
		modalNewScene();
	}

	/*
	*	dir ��� string, wstring ������ �ʿ��ϴ�. 
	*	project load ���¸� ��� Ȯ���� ���ΰ� �����ؾ� �Ѵ�.
	*/
	void MenuBarPanel::modalNewProject()
	{
		static std::string inputName;
		static std::string inputDir;
		static std::string curPath;

		if (m_bModalNewProject)
		{
			ImGui::OpenPopup("##new_project");

			m_bModalNewProject = false;
		
			inputName = "NewApplication";
			// '/'�� '\'�� �ٲٰ�, �������� �ش� ��ū�� �������� ���ڴ�.
			inputDir = std::filesystem::current_path().string() + '/';
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("##new_project", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
		{
			// title
			{
				// �ؽ�Ʈ�� �� ũ�� ����� �ʹ�.
				// �׷��� ���� �ʴ�.
				ImGui::Text("Create New Proejct");
				ImGui::Dummy(ImVec2(0.0f, 30.0f));
			}

			// project naming
			{
				ImGui::Text("Project Name");
				ImGui::Dummy(ImVec2(0.0f, 5.0f));
				ImGui::PushItemWidth(400);
				// ��Ŀ���� �ְ� �ʹ�.
				ImGui::InputText("##input_name", &inputName, ImGuiInputTextFlags_AutoSelectAll);
				ImGui::PopItemWidth();
				ImGui::NewLine();
			}

			// set location
			{
				ImGui::Text("Location");
				ImGui::Dummy(ImVec2(0.0f, 5.0f));
				ImGui::PushItemWidth(400);
				ImGui::InputText("##input_dir", &inputDir, ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemWidth();
				ImGui::SameLine(0.0f, 5.0f);
				if (ImGui::Button("..."))
				{
					BROWSEINFO bi;
					ZeroMemory(&bi, sizeof(BROWSEINFO));

					// A, W �����ε� �ϴ�.
					bi.hwndOwner = NULL;// m_pEditor->GetWindowHandle();
					bi.pidlRoot		= NULL;
					bi.lpszTitle	= L"������Ʈ ���� ���� ����";
					bi.ulFlags = BIF_EDITBOX | BIF_NEWDIALOGSTYLE | BIF_BROWSEINCLUDEURLS;
					bi.lpfn			= BrowseCallbackProc;
					// ���̴�. /�� �ȸ�����. \\�� �ؾ� �Ѵ�.
					// �̰� ��θ� �����ϴ� c++�� winapi�� �����ε� �ϴ�.
					std::wstring path = //Dive::Util::Util::String::Utf8ToUtf16(inputDir.c_str());
						std::filesystem::current_path().generic_wstring();
					bi.lParam = //(LPARAM)L"C:\\";
								//(LPARAM)(path.c_str());
								//(LPARAM)L"C:\\Dev\\Projects\\DiveGames\\Editor";
								(LPARAM)(path.c_str());

					LPITEMIDLIST pSelected = ::SHBrowseForFolder(&bi);
					if (pSelected != NULL)
					{
						WCHAR temp[MAX_PATH] = { 0 };
						if (::SHGetPathFromIDList(pSelected, temp))
						{
							//inputDir = temp;
						}
					}
				}
			}

			ImGui::Dummy(ImVec2(0.0f, 100.0f));

			// Buttons
			{
				// �� ��ư�� ���� ���� �ϰ� �ʹ�.

				if (ImGui::Button("Cancel", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine(0.0f, 5.0f);

				// �̸� Ȥ�� ��ΰ� ����ִٸ� �ƿ� ��Ȱ��ȭ�ȴ�.
				static bool enabled = true;
				if (inputName.empty())
					enabled = false;
				else
					enabled = true;

				if (!enabled)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}


				if (ImGui::Button("Create", ImVec2(120, 0)))
				{
					// C:\\�� �� ������ �� �� �ִ�. 
					// => �ƴϳ�...
					curPath = inputDir + "\\" + inputName;

					if (0 == _access_s(curPath.c_str(), 0))
					{
						ImGui::OpenPopup("##warn_box");
					}
					else
					{
						// 1. ���� ���� ������Ʈ Ȥ�� ������ �ִٸ� + ���� ������ �ִٸ�
						// �ٽ� modal�� ���â�� ����� �Ѵ�.
						// => �̰� ���� �Ǻ��ؾ� ����...?

						// 2. Project�� �����ϴ� �Լ��� ȣ���Ѵ�.
						// ���������� ������ �����ϰ� Project file���� ���� ���̴�.
						// �� ������ ���� �����ϴ� �� ���� �Լ��� ����� ȣ���ϴ� ���� ���� �� ����.
						// �׷��� �ٸ� ������ �Ⱦ��� �� ����...
						// Project class�� ���� �� serialization �� ���ΰ�?
						// ������ ����ȭ�� �� ���ΰ�?
						APP_TRACE("create project: {:s}", curPath);

						ImGui::CloseCurrentPopup();
					}
				}

				// ����˾� �κ��� Button ó�� ���ο� ���� �ȱ׷�����.
				{
					ImVec2 center = ImGui::GetMainViewport()->GetCenter();
					ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

					// �̰� �߰��ϴ� Ÿ��Ʋ�ٿ� ���� ��ư�� ����� ��?
					bool unused_open = true;
					if (ImGui::BeginPopupModal("##warn_box", &unused_open, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Text("already exist.\nPlease, input other directory or project name.");

						ImGui::Dummy(ImVec2(0.0f, 50.0f));

						if (ImGui::Button("OK", ImVec2(120.0, 0)))
							ImGui::CloseCurrentPopup();

						ImGui::EndPopup();
					}
				}

				if (!enabled)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}
			}

			ImGui::EndPopup();
		}
	}

	// ��ư�� ���� ���ĵǵ��� �ϰ� �ʹ�.
	void MenuBarPanel::modalNewScene()
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

				auto newScene = SceneManager::GetInstance().CreateScene(sceneName);
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
					SceneManager::GetInstance().SetActiveScene(newScene);
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