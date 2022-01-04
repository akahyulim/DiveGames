#include "MenuBar.h"
#include "Editor.h"
#include "Geometry.h"
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_internal.h"
#include <Windows.h>
#include <Commdlg.h>

using namespace DiveEngine;

namespace DiveEditor
{
	MenuBar::MenuBar(Editor* pEditor)
		: Widget(pEditor)
	{
		m_Title				= "MenuBar";
		m_bWindow			= false;

		m_bModalNewProject	= false;
		m_bModalNewScene	= false;
	}

	// Scene이 없을 때 GameObject를 생성할 수 없는데 그냥 시도한다.
	void MenuBar::TickAlways()
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
						// 현재 저장되지 않은 프로젝트가 로드 중일 때
						// 1. 새로운 프로젝트 생성창 팝업
						// 2. 생성창에서 만들기 버튼을 클릭했을 경우
						// 저장되지 않은 사항이 있다는 메시지와 함께 저장, 저장안함, 취소 세 개의 버튼으로 구성된 창 팝업
						// 현재 프로젝트가 저장된 상태라면 바로 새로운 프로젝트가 생성

						// 생성창
						// 경로 설정
						// 프로젝트 이름 입력
						// 취소, 만들기 버튼

						m_bModalNewProject = true;
					}

					if (ImGui::MenuItem("Scene"))
					{
						// 생성창
						// Scene 이름 입력
						// 취소, 만들기 버튼
						// 프로젝트에 포함되지 않았다면 경로도 설정해 줘야지...
						// 결국 단일 Scene 생성을 가능하게 할지도 관련있다.
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Open"))
				{
					if (ImGui::MenuItem("Project"))
					{
						// 원래는 특정 폴더를 열어야 한다.
						// 1 순위는 저장된 시작 폴더
						// 2 순위는 Editor의 실행 폴더
						// 일단은 Editor의 실행 폴더를 열도록 한다.
						auto dir = DiveEngine::FileSystemHelper::GetWorkingDirectory();

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
							// 선택된 파일 확인
							APP_TRACE("Open Project: {:s}", targetFile);

							// 현재 다른 프로젝트가 실행 중이고
							// 변경 사항이 존재할 경우
							// 메시지와 함께 저장, 저장안함, 취소 버튼의 팝업 창이 뜬다.
							
							// 이후 선택된 프로젝트 파일을 로드한다.

							// 프로젝트 폴더를 문자열로 저장하면 더 좋을 것 같다.
						}
					}

					if (ImGui::MenuItem("Scene"))
					{
						// 여긴 우선 순위가 다르다.
						// 1 순위는 현재 프로젝트 폴더의 Assets\Scenes => 즉, 프로젝트 실행시 저장이 필요하다.
						// 2 순위는 역시나 Editor의 위치 폴더
						auto dir = DiveEngine::FileSystemHelper::GetWorkingDirectory();

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
							// 선택된 파일 확인
							APP_TRACE("Open Scene: {:s}", targetFile);

							// 현재 다른 프로젝트가 실행 중이고
							// 변경 사항이 존재할 경우
							// 메시지와 함께 저장, 저장안함, 취소 버튼의 팝업 창이 뜬다.

							// 이후 선택된 프로젝트 파일을 로드한다.
						}

					}

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Close"))
				{
					// 현재 scene 종료
				}

				if (ImGui::MenuItem("Close Project"))
				{
					// 현재 project 종료
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save"))
				{
					// Scene 저장이겠지...
					// => VS도 파일 저장이다.
				}

				if (ImGui::MenuItem("Save As..."))
				{
					// OPENFILENAME, GetSaveFileName 

					auto dir = DiveEngine::FileSystemHelper::GetWorkingDirectory();

					// 현재 Scene 이름으로 설정해야 한다.
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
						// 현재 폴더에 동일한 이름의 파일이 존재하는지 확인해야 한다.
						// 이름 변경 자체는 또 직접 해야하나 보다.
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					// 역시 변경된 사항이 있을 경우
					// 메시지와 함께 저장, 저장안함, 취소 버튼의 팝업 창이 뜬다.
					// Scene 기준이다.

					// 깔끔한 종료가 인가... 아닌가...
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
							// 팝업창을 띄워 Dirty면 변경점을 저장할 것인지 묻는다.
						}
					}

					// 팝업을 띄워야 하는데 방법이 있나 보다.
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
						// 팝업
						// 기존 Scene에 변경 사항이 존재합니다. 저장하시겠습니까?
						}
					}

					// 디렉토리 탐색기를 연다.
					{
						std::string filepath = "../Assets/Scenes/MeshTestScene.scene";//"../Assets/Scenes/default.scene";
						SceneManager::GetInstance().LoadScene(filepath);
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save"))
				{
					// 동일한 이름의 파일이 있는지 확인한다.
					// 만약 있다면 덮어씌울 것인지 물어야 한다.
					{
						std::string filepath = "../Assets/Scenes/" + m_pScene->GetName() + ".scene";
						//Scene::GetGlobalScene().SaveToFile(filepath);
						m_pScene->SaveToFile(filepath);

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
					//if (Scene::GetGlobalScene().IsDirty())
					if(m_pScene->IsDirty())
					{

					}
					// 하지만 추후 Project의 변경사항까지 확인해야 한다.

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
							std::vector<DiveEngine::Vertex_StaticMesh> vertices;
							std::vector<uint32_t> indices;
							Utility::CreateCube(vertices, indices);

							pMesh = new DiveEngine::StaticMesh();
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
							std::vector<DiveEngine::Vertex_StaticMesh> vertices;
							std::vector<uint32_t> indices;
							Utility::CreateSphere(vertices, indices);

							pMesh = new DiveEngine::StaticMesh();
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
							std::vector<DiveEngine::Vertex_StaticMesh> vertices;
							std::vector<uint32_t> indices;
							Utility::CreateCylinder(vertices, indices);

							pMesh = new DiveEngine::StaticMesh();
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
							std::vector<DiveEngine::Vertex_StaticMesh> vertices;
							std::vector<uint32_t> indices;
							Utility::CreatePlane(vertices, indices);

							pMesh = new DiveEngine::StaticMesh();
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
							std::vector<DiveEngine::Vertex_StaticMesh> vertices;
							std::vector<uint32_t> indices;
							Utility::CreateQuad(vertices, indices);

							pMesh = new DiveEngine::StaticMesh();
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

		//= Popups ============================
		modalNewProject();
		modalNewScene();

	}

	// 크기를 미리 만들어 놓고 싶다.
	void MenuBar::modalNewProject()
	{
		static char projectName[32] = { 0, };
		static char projectLoc[128] = { 0, };

		if (m_bModalNewProject)
		{
			ImGui::OpenPopup("##Create New Project");
			m_bModalNewProject = false;
			ZeroMemory(projectName, sizeof(projectName));

			// 일단 현재 실행 폴더로...
			auto curDir = DiveEngine::FileSystemHelper::GetWorkingDirectory();
			strcpy_s(projectLoc, curDir.c_str());
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		//ImGui::SetNextWindowSize(ImVec2(500, 300));

		if (ImGui::BeginPopupModal("##Create New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			// title
			{
				// 좀 크게 만들고 싶다.
				ImGui::Text("Create New Proejct");
				ImGui::NewLine();
				ImGui::NewLine();
			}

			// project naming
			{
				ImGui::Text("Project Name");
				ImGui::NewLine();	// 이건 좀 줄이고 싶다.
				ImGui::PushItemWidth(400);
				ImGui::InputText("##name", projectName, IM_ARRAYSIZE(projectName));
				ImGui::PopItemWidth();
				ImGui::NewLine();
			}

			// set location
			{
				ImGui::Text("Location");
				ImGui::NewLine();	// 이것도 좀 줄이고 싶다.
				ImGui::PushItemWidth(400);
				// 일단 디폴트 폴더가 존재해야 한다. 아래의 Button은 이를 기준으로 검색한다.
				ImGui::InputText("##loc", projectLoc, IM_ARRAYSIZE(projectLoc));
				ImGui::PopItemWidth();
				ImGui::SameLine(0.0f, 5.0f);
				if(ImGui::Button("...")) 
				{
					// 디렉토리 창을 열어 선택한 경로를 복사해온다.
					// 이건 파일 열기 대화상자랑 좀 다르다.
				}
			}

			ImGui::NewLine();
			ImGui::NewLine();
			ImGui::NewLine();
			ImGui::NewLine();

			// Buttons
			{	
				// 둘 다 우측 정렬하고 싶다. 그럴려면 적어도 크기는 고정되어야 하나...?
				if (ImGui::Button("Cancel", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine(0.0f, 5.0f);

				// 이름 혹은 경로가 비어있다면 아에 비활성화된다.
				if (ImGui::Button("Create", ImVec2(120, 0)))
				{
					// 이름 확인
					// 동일한 이름은 안된다.
					// 동일한 이름이 존재한다며 경고창을 띄운다. 확인 버튼만 있다.

					// 저장이 필요한지 확인 필요
					// 저장이 필요하다면 다른 팝업을 또 띄워야 한다.
					// 저장 대상이 Scene으로 한정되니... 하나를 만들면 돌려 쓸 수 있을 것 같다.
					// 다만 modal에서 다시 modal이 되나....?

					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}
	}

	// 버튼이 우측 정렬되도록 하고 싶다.
	void MenuBar::modalNewScene()
	{
		static char sceneName[16] = { 0, };
	
		if (m_bModalNewScene)
		{
			ImGui::OpenPopup("New Scene");
			m_bModalNewScene = false;
			ZeroMemory(sceneName, sizeof(sceneName));
		}

		// 이렇게 하니 움직이지 않는다.
		auto& io = ImGui::GetIO();
		//ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("New Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::NewLine();
			ImGui::Text("Input New Scene Name");
			ImGui::NewLine();

			ImGui::InputText("##edit", &sceneName[0], IM_ARRAYSIZE(sceneName));

			ImGui::NewLine();

			// 이렇게 쓰는게 아닌가?
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
				// 빈 문자열은 안된다.

				auto newScene = SceneManager::GetInstance().CreateScene(sceneName);
				if(!newScene)
				{
					// 이것두 m_b로 만들어야 할 것 같다.
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
					// 이걸 직접 하는게 아직 좀 그렇다.
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