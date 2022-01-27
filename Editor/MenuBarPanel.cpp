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

	// Scene이 없을 때 GameObject를 생성할 수 없는데 그냥 시도한다.
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

					auto dir = std::filesystem::current_path().string() + '/';

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

	/*
	*	dir 제어에 string, wstring 구분이 필요하다. 
	*	project load 상태를 어떻게 확인할 것인가 생각해야 한다.
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
			// '/'를 '\'로 바꾸고, 마지막엔 해당 토큰이 없었으면 좋겠다.
			inputDir = std::filesystem::current_path().string() + '/';
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("##new_project", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
		{
			// title
			{
				// 텍스트를 좀 크게 만들고 싶다.
				// 그런데 쉽지 않다.
				ImGui::Text("Create New Proejct");
				ImGui::Dummy(ImVec2(0.0f, 30.0f));
			}

			// project naming
			{
				ImGui::Text("Project Name");
				ImGui::Dummy(ImVec2(0.0f, 5.0f));
				ImGui::PushItemWidth(400);
				// 포커스를 주고 싶다.
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

					// A, W 문제인듯 하다.
					bi.hwndOwner = NULL;// m_pEditor->GetWindowHandle();
					bi.pidlRoot		= NULL;
					bi.lpszTitle	= L"프로젝트 생성 폴더 선택";
					bi.ulFlags = BIF_EDITBOX | BIF_NEWDIALOGSTYLE | BIF_BROWSEINCLUDEURLS;
					bi.lpfn			= BrowseCallbackProc;
					// 헐이다. /는 안먹힌다. \\로 해야 한다.
					// 이건 경로를 구분하는 c++과 winapi의 차이인듯 하다.
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
				// 두 버튼을 우측 정렬 하고 싶다.

				if (ImGui::Button("Cancel", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine(0.0f, 5.0f);

				// 이름 혹은 경로가 비어있다면 아에 비활성화된다.
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
					// C:\\일 때 문제가 될 수 있다. 
					// => 아니네...
					curPath = inputDir + "\\" + inputName;

					if (0 == _access_s(curPath.c_str(), 0))
					{
						ImGui::OpenPopup("##warn_box");
					}
					else
					{
						// 1. 현재 열린 프로젝트 혹은 파일이 있다면 + 변경 사항이 있다면
						// 다시 modal로 경고창을 띄워야 한다.
						// => 이건 뭘로 판별해야 하지...?

						// 2. Project를 생성하는 함수를 호출한다.
						// 내부적으로 폴더를 생성하고 Project file까지 만들 것이다.
						// 이 곳에서 직접 생성하는 것 보다 함수를 만들고 호출하는 편이 나을 것 같다.
						// 그런데 다른 곳에선 안쓰일 것 같다...
						// Project class를 만든 후 serialization 할 것인가?
						// 간단한 파일화만 할 것인가?
						APP_TRACE("create project: {:s}", curPath);

						ImGui::CloseCurrentPopup();
					}
				}

				// 모달팝업 부분이 Button 처리 내부에 들어가면 안그려진다.
				{
					ImVec2 center = ImGui::GetMainViewport()->GetCenter();
					ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

					// 이걸 추가하니 타이틀바에 종료 버튼이 생기는 듯?
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

	// 버튼이 우측 정렬되도록 하고 싶다.
	void MenuBarPanel::modalNewScene()
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