#include "Editor.h"
#include "Project/Project.h"
#include "EditorView/EditorView.h"
#include "EditorView/SceneView.h"
#include "EditorView/GameView.h"
#include "EditorView/HierarchyView.h"
#include "EditorView/InspectorView.h"
#include "EditorView/ProjectView.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK EditorMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

	switch (msg)
	{
	case WM_NCHITTEST:
	{
		RECT rect;
		GetWindowRect(hWnd, &rect);
		POINT pt = { LOWORD(lParam), HIWORD(lParam) };

		// 드래그 가능한 영역 세분화
		int border_margin = 300;
		if (pt.y >= rect.top && pt.y < rect.top + 30) // 상단 30픽셀 영역
		{
			if (pt.x < rect.left + border_margin || pt.x > rect.right - border_margin) // 좌우 여백
			{
				break;
			}
			return HTCAPTION; // 제목 표시줄 영역으로 처리
		}
		break;
	}
	case WM_SIZE:
	{
		auto data = std::make_unique<std::pair<uint32_t, uint32_t>>(LOWORD(lParam), HIWORD(lParam));
		DV_FIRE_EVENT_DATA(Dive::eEventType::WindowResized, (void*)data.get());
		return 0;
	}
	case WM_CLOSE:
	{
		Dive::Editor::GetInstance().Exit();
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

namespace Dive
{
	static uint16_t s_SceneCounter = 0;

	Editor::Editor()
		: m_pActiveProject(nullptr)
		, m_pEditScene(nullptr)
		, m_bProjectChanged(false)
		, m_bShowSaveChangesPopup(false)
		, m_bShowNewProjectPopup(false)
		, m_bShowNewScenePopup(false)
	{
		// 로그 매니져 설정
		Dive::LogManager::SetFilename("dive_editor.log");

		// 엔진 초기화 및 윈도우 설정
		// => Initialize는 매개변수 없이 호출하고
		GEngine->Initialize(::GetModuleHandle(nullptr), 1600, 900, L"Dive Editor");
		// => 여기에 윈도우 크기 변경 함수를 호출하도록 하자.
		Window::SetMessageCallback((LONG_PTR)EditorMessageHandler);

		// ImGui 초기화
		{
			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-WorldView / Platform Windows

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();

			ImGuiStyle& style = ImGui::GetStyle();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				style.WindowRounding = 0.0f;
				style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			}

			// Setup Platform/Renderer backends
			ImGui_ImplWin32_Init(Window::GetWindowHandle());//GEngine->GetWindowHandle());
			ImGui_ImplDX11_Init(Graphics::GetDevice(), Graphics::GetDeviceContext());//GEngine->GetDevice(), GEngine->GetDeviceContext());

			// 스타일 수정
			{
				auto& colors = ImGui::GetStyle().Colors;
				colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

				// Headers
				colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
				colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
				colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

				// Buttons
				colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
				colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
				colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

				// Frame BG
				colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
				colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
				colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

				// Tabs
				colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
				colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
				colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
				colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
				colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

				// Title
				colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
				colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
				colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
			}

			// load fonts
			float fontSize = 19.0f;
			io.FontDefault = m_Fonts[static_cast<size_t>(eFontTypes::Normal)] =
				io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/NanumBarunGothic.ttf", fontSize, nullptr, io.Fonts->GetGlyphRangesKorean());
			m_Fonts[static_cast<size_t>(eFontTypes::Bold)] = io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/NanumBarunGothicBold.ttf", fontSize, nullptr, io.Fonts->GetGlyphRangesKorean());
			m_Fonts[static_cast<size_t>(eFontTypes::Large)] = io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/NanumBarunGothic.ttf", fontSize * 1.5f, nullptr, io.Fonts->GetGlyphRangesKorean());
			m_Fonts[static_cast<size_t>(eFontTypes::Large_Bold)] = io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/NanumBarunGothicBold.ttf", fontSize * 1.5f, nullptr, io.Fonts->GetGlyphRangesKorean());
		}

		// create views
		EditorView::CreateView<SceneView>();
		EditorView::CreateView<GameView>();
		EditorView::CreateView<HierarchyView>();
		EditorView::CreateView<InspectorView>();
		EditorView::CreateView<ProjectView>();

		// 임시 프로젝트 및 씬 생성
		{
			if (std::filesystem::exists("NewProject"))
			{
				m_pActiveProject = Project::LoadFromFile("NewProject/NewProject.dive");
				if (m_pActiveProject)
				{
					auto& openedSceneName = m_pActiveProject->GetOpenedSceneName();
					std::filesystem::path worldFilePath;
					if (!openedSceneName.empty())
						worldFilePath = m_pActiveProject->GetScenesPath() / (openedSceneName + ".dive");
					else
						worldFilePath = m_pActiveProject->GetScenesPath() / "NewScene.dive";

					m_pEditScene = SceneManager::LoadFromFile(worldFilePath);
					//m_pEditScene = new Scene(openedSceneName);
					//SceneSerializer serializer(m_pEditScene);
					//serializer.Deserialize(worldFilePath);

					createEditorOnly();
				}
			}
			else
			{
				m_pActiveProject = Project::New(std::filesystem::current_path() / "NewProject/NewProject.dive");
				if (m_pActiveProject)
				{
					Project::SaveToFile(m_pActiveProject);

					m_pEditScene = SceneManager::CreateScene();
					//m_pEditScene = new Scene("NewScene");
					
					createEditorOnly();
					//auto pEditorCamera = m_pEditScene->CreateGameObject("EditorCamera");
					//pEditorCamera->SetTag("EditorOnly");
					//pEditorCamera->AddComponent<Camera>();
					auto pMainCamera = m_pEditScene->CreateGameObject("MainCamera");
					pMainCamera->AddComponent<Camera>();
					
					//SceneSerializer serializer(m_pEditScene);
					//serializer.Serialize(GetActiveSceneFilePath());
					SceneManager::SaveToFile(GetActiveSceneFilePath());
				}
			}
			s_SceneCounter++;
		}

		DV_SUBSCRIBE_EVENT(Dive::eEventType::PostRender, DV_EVENT_HANDLER(OnPostRender));
	}

	Editor::~Editor()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		GEngine->Shutdown();
	}

	int Editor::Run()
	{
		// Engine에서 분리한 후
		// 위치가 상당히 애매해졌다.
		if (m_pEditScene)
			m_pEditScene->Tick();

		GEngine->Tick();

		return 0;
	}

	void Editor::OnPostRender()
	{
		// Begin UI
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// Note: Switch this to true to enable dockspace
			static bool dockspaceOpen = true;
			static bool opt_fullscreen_persistant = true;
			bool opt_fullscreen = opt_fullscreen_persistant;
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
			// because it would be confusing to have two docking targets within each others.
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			if (opt_fullscreen)
			{
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}

			// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				window_flags |= ImGuiWindowFlags_NoBackground;

			// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
			// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
			// all active windows docked into it will lose their parent and become undocked.
			// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
			// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
			ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			// DockSpace
			ImGuiIO& io = ImGui::GetIO();
			ImGuiStyle& style = ImGui::GetStyle();
			float minWinSizeX = style.WindowMinSize.x;
			style.WindowMinSize.x = 200.0f;	// dock space에서의 최소 크기...
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}
			style.WindowMinSize.x = minWinSizeX;
		}

		// Draw UI
		{
			drawMainManubar();
			showPopups();

			EditorView::DrawViews();
		}

		// End UI
		{
			ImGui::End();

			auto pDefaultRenderTargetView = Graphics::GetBackbufferRTV();
			const float clear_color_with_alpha[4]{ 0.1f, 0.1f, 0.1f, 0.0f };
			Graphics::GetDeviceContext()->OMSetRenderTargets(1, &pDefaultRenderTargetView, NULL);
			Graphics::GetDeviceContext()->ClearRenderTargetView(pDefaultRenderTargetView, clear_color_with_alpha);

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			ImGuiIO& io = ImGui::GetIO(); (void)io;
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}
	}

	ImFont* Editor::GetFont(eFontTypes type)
	{
		return m_Fonts[static_cast<size_t>(type)];
	}

	void Editor::drawMainManubar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Create Project"))
				{

				}
				if (ImGui::MenuItem("Open Project"))
				{

				}
				if (ImGui::MenuItem("Save Project"))
				{

				}

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene"))
				{

				}
				if (ImGui::MenuItem("Save Scene"))
				{

				}

				if (ImGui::MenuItem("Save Scene As..."))
				{

				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					Exit();
				}

				ImGui::EndMenu();
			}
			
			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tool"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				ImGui::EndMenu();
			}

			if (m_pActiveProject)
			{
				auto center = ImGui::GetColumnWidth() * 0.5f;
				ImGui::SameLine(center);
				
				ImGui::PushFont(m_Fonts[(size_t)eFontTypes::Bold]);
				ImGui::Text(m_pActiveProject->GetName().c_str());
				ImGui::PopFont();
			}

			float availableWidth = ImGui::GetContentRegionAvail().x;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availableWidth - 250); // 버튼 너비만큼 조정
			if (ImGui::Button("Minimize"))
			{
				Window::Minimize();
			}
			ImGui::SameLine();
			if (ImGui::Button(Window::IsMaximize() ? "Restore" : "Maximize"))
			{
				if (Window::IsMaximize())
				{
					Window::Restore();
				}
				else
				{
					Window::Maximize();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Close"))
			{
				Exit();
			}

			ImGui::EndMainMenuBar();
		}
		/*
		if (ImGui::BeginMainMenuBar())
		{
			// FILE
			if (ImGui::BeginMenu(u8"파일"))
			{
				if (ImGui::MenuItem(u8"새로운 씬", nullptr, nullptr, m_pActiveProject))
				{
					NewScene();
				}
				if (ImGui::MenuItem(u8"씬 열기", nullptr, nullptr, m_pActiveProject))
				{
					OpenScene();
				}
				
				ImGui::Separator();
				
				if (ImGui::MenuItem(u8"씬 저장", nullptr, nullptr, m_pEditScene && m_pEditScene->IsDirty()))
				{
					SaveScene();
				}
				if (ImGui::MenuItem(u8"다른 이름으로 씬 저장", nullptr, nullptr, m_pEditScene))
				{
					SaveSceneAs();
				}

				ImGui::Separator();
				
				if (ImGui::MenuItem(u8"새로운 프로젝트"))
				{
					NewProject();
				}
				if (ImGui::MenuItem(u8"프로젝트 열기"))
				{
					OpenProject();
				}
				if (ImGui::MenuItem(u8"프로젝트 저장", nullptr, nullptr, m_pActiveProject && m_bProjectChanged))
				{
					SaveProject();
				}

				ImGui::Separator();

				if (ImGui::MenuItem(u8"끝내기"))
				{
					Exit();
				}
				ImGui::EndMenu();
			}

			// Game Object
			if (ImGui::BeginMenu(u8"게임 오브젝트"))
			{
				if (ImGui::MenuItem(u8"빈 게임 오브젝트", nullptr, nullptr, (m_pEditScene != nullptr)))
				{
					auto pNewGameObject = m_pEditScene->CreateGameObject();
				}

				if (ImGui::BeginMenu(u8"3D 오브젝트", (m_pEditScene != nullptr)))
				{
					if (ImGui::MenuItem(u8"상자"))
					{
						
					}

					if (ImGui::MenuItem(u8"구"))
					{
						
					}

					if (ImGui::MenuItem(u8"평면"))
					{
						
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu(u8"라이트", (m_pEditScene != nullptr)))
				{
					if (ImGui::MenuItem(u8"디렉셔널 라이트"))
					{
					}

					if (ImGui::MenuItem(u8"포인트 라이트"))
					{
					}

					if (ImGui::MenuItem(u8"스포트 라이트"))
					{
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu(u8"카메라", (m_pEditScene != nullptr)))
				{
					if (ImGui::MenuItem(u8"원근 투영"))
					{
						auto pCamera = m_pEditScene->CreateGameObject("Camera");
						pCamera->AddComponent<Camera>();
					}

					if (ImGui::MenuItem(u8"직교 투영"))
					{
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			// Component
			// SelectedGameObject가 존재할 때 활성화
			if (ImGui::BeginMenu(u8"컴포넌트"))
			{
				if (ImGui::MenuItem(u8"렌더러블", nullptr, nullptr, (m_pSelectedGameObject != nullptr)))
				{
				}

				if (ImGui::MenuItem(u8"라이트", nullptr, nullptr, (m_pSelectedGameObject != nullptr)))
				{
				}

				ImGui::EndMenu();
			}

			// Views
			if (ImGui::BeginMenu(u8"창"))
			{
				static bool showView = true;

				/*
				showView = m_Panels[static_cast<size_t>(ePanelTypes::SceneView)]->IsVisible();
				ImGui::Checkbox("SceneView", &showView);
				m_Panels[static_cast<size_t>(ePanelTypes::SceneView)]->SetVisible(showView);

				showView = m_Panels[static_cast<size_t>(ePanelTypes::GameView)]->IsVisible();
				ImGui::Checkbox("GameView", &showView);
				m_Panels[static_cast<size_t>(ePanelTypes::GameView)]->SetVisible(showView);

				showView = m_Panels[static_cast<size_t>(ePanelTypes::HierarchyView)]->IsVisible();
				ImGui::Checkbox("HierarchyView", &showView);
				m_Panels[static_cast<size_t>(ePanelTypes::HierarchyView)]->SetVisible(showView);

				showView = m_Panels[static_cast<size_t>(ePanelTypes::InspectorView)]->IsVisible();
				ImGui::Checkbox("InspectorView", &showView);
				m_Panels[static_cast<size_t>(ePanelTypes::InspectorView)]->SetVisible(showView);

				showView = m_Panels[static_cast<size_t>(ePanelTypes::ProjectView)]->IsVisible();
				ImGui::Checkbox("ProjectView", &showView);
				m_Panels[static_cast<size_t>(ePanelTypes::ProjectView)]->SetVisible(showView);

				ImGui::EndMenu();
			}

			// Help
			if (ImGui::BeginMenu(u8"도움말"))
			{
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
		*/
	}
	
	void Editor::showPopups() 
	{
		// SaveChangesPopup
		if (m_bShowSaveChangesPopup) 
		{ 
			ImGui::OpenPopup("Save Changes?"); 
			m_bShowSaveChangesPopup = false; 
		} 
		
		if (ImGui::BeginPopupModal("Save Changes?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) 
		{
			ImGui::Text(u8"변경사항이 존재합니다. 저장하시겠습니까?");
			// 변경 사항이 있는 파일을 표시
			if (ImGui::Button(u8"저장", ImVec2(120, 0)))
			{
				// 이건 미비하다. 다수의 씬가 수정되었을 수 있다.
				// => 그렇다면 결국 씬별로 수정여부를 판단해야 한다는 건데?
				if (m_pEditScene && m_pEditScene->IsDirty())
				{
					//SceneSerializer serializer(m_pEditScene);
					//serializer.Serialize(GetActiveSceneFilePath());
					SceneManager::SaveToFile(GetActiveSceneFilePath());
					m_pActiveProject->SetOpenedSceneName(m_pEditScene->GetName());
				}
				if(m_bProjectChanged)
					Project::SaveToFile(m_pActiveProject);
				if (m_OnConfirm) m_OnConfirm();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"저장 안함", ImVec2(120, 0)))
			{
				if (m_OnConfirm) m_OnConfirm();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"취소", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup(); 
		} 
	
		// NewProjectPopup
		if (m_bShowNewProjectPopup)
		{
			ImGui::OpenPopup("New Project");
			m_bShowNewProjectPopup = false;
		}

		if (ImGui::BeginPopupModal("New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::PushFont(GetFont(eFontTypes::Large_Bold));
			ImGui::Spacing();
			ImGui::Text(u8"새로운 프로젝트");
			ImGui::PopFont();

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			// 생성 프로젝트 이름
			static std::string projectName = "New_Project";
			char projectNameBuffer[256];
			strncpy_s(projectNameBuffer, projectName.c_str(), sizeof(projectNameBuffer));
			ImGui::Text(u8"프로젝트 이름");
			ImGui::PushItemWidth(400);
			if (ImGui::InputText("##project_name", projectNameBuffer, sizeof(projectNameBuffer)))
			{
				projectName = std::string(projectNameBuffer);
			}
			ImGui::PopItemWidth();

			ImGui::Spacing();

			// 생성 프로젝트 위치
			static std::filesystem::path projectPath = std::filesystem::current_path();
			char projectPathBuffer[512];
			strncpy_s(projectPathBuffer, projectPath.string().c_str(), sizeof(projectPathBuffer));
			ImGui::Text(u8"위치");
			ImGui::PushItemWidth(400);
			if (ImGui::InputText("##location", projectPathBuffer, sizeof(projectPathBuffer)))
			{
				projectPath = std::string(projectPathBuffer);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			if (ImGui::Button(u8"..."))
			{
				projectPath = FileUtils::SelectFolder();
			}

			//ImVec2 window_size = ImGui::GetWindowSize();
			//ImGui::SetCursorPos(ImVec2(window_size.x - 220, window_size.y - 40));
			ImGui::Spacing();

			if (ImGui::Button(u8"취소", ImVec2(100, 30)))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"만들기", ImVec2(100, 30)))
			{
				// 해당 디렉토리가 이미 존재한다면 경고 대화상자를 띄워야 한다.
				if (std::filesystem::exists(projectPath / projectName))
				{
					ImGui::OpenPopup("Exist same path");
					//::MessageBox(nullptr, L"동일한 프로젝트 디렉토리가 이미 존재하여 생성할 수 없습니다.", L"경고", MB_OK);
				}
				else
				{
					// 새로운 프로젝트 생성
					m_pActiveProject = Project::New(projectPath / projectName / (projectName + ".dive"));

					// 새로운 씬 생성
					//m_pEditScene = new Scene("NewScene");
					//SceneSerializer serializer(m_pEditScene);
					//serializer.Serialize(GetActiveSceneFilePath());
					m_pEditScene = SceneManager::CreateScene();
					SceneManager::SaveToFile(GetActiveSceneFilePath());

					m_pActiveProject->SetOpenedSceneName(m_pEditScene->GetName());

					ImGui::CloseCurrentPopup();
				}
			}

			if (ImGui::BeginPopupModal("Exist same path", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text(u8"이미 존재하는 프로젝트 디렉토리에 생성할 수 없습니다.");
				if (ImGui::Button(u8"확인"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::EndPopup();
		}
	}

	void Editor::activateSaveChanges(std::function<void()> confirmCallback)
	{
		m_OnConfirm = confirmCallback;
		m_bShowSaveChangesPopup = true;
	}

	// 이름은 추후 변경 염두
	void Editor::createEditorOnly()
	{
		if (!m_pEditScene)
			return;

		// 카메라
		m_pEditorCamera = m_pEditScene->CreateGameObject("EditorCamera");
		auto pCameraCom = m_pEditorCamera->AddComponent<Camera>();
		m_pEditorCamera->SetTag("EditorOnly");

		// 그리드
	}

	void Editor::NewScene()
	{
		auto OnCreateNewScene = [this]() {
			SceneManager::UnloadScene();	// 이게 문제다.
			
			//m_pEditScene = new Scene("NewScene_" + std::to_string(s_SceneCounter++));	// 이걸론 고유한 이름을 확보할 수 없다.
			m_pEditScene = SceneManager::CreateScene("NewScene_" + std::to_string(s_SceneCounter++));

			// 메인 카메라
			auto pMainCameraObj = m_pEditScene->CreateGameObject("Main Camera");
			pMainCameraObj->AddComponent<Camera>();
			
			// 에디터 카메라
			// 여기보단 다른 곳을 찾아야 할 듯 하다.
			//auto pEditorCameraObj = m_pEditScene->CreateGameObject("Editor Camera");
			//pEditorCameraObj->AddComponent<Camera>();
			//pEditorCameraObj->SetTag("EditorOnly");
			createEditorOnly();

			//SceneSerializer serializer(m_pEditScene);
			//serializer.Serialize(GetActiveSceneFilePath());
			SceneManager::SaveToFile(GetActiveSceneFilePath());

			m_pActiveProject->SetOpenedSceneName(m_pEditScene->GetName());
			m_bProjectChanged = true;
			};

		if (m_pEditScene->IsDirty())
		{
			activateSaveChanges(OnCreateNewScene);
		}
		else
		{
			OnCreateNewScene();
		}
	}

	void Editor::OpenScene()
	{
		auto OnOpenScene = [this]() {
			SceneManager::UnloadScene();

			auto filePath = FileUtils::OpenFile(
				"Dive Scene (*.dive)\0*.dive\0",
				//GEngine->GetWindowHandle(),
				Window::GetWindowHandle(),
				m_pActiveProject->GetScenesPath().string().c_str()
			);
			if (filePath.empty()) return;

			//m_pEditScene = new Scene;
			//SceneSerializer serializer(m_pEditScene);
			//serializer.Deserialize(filePath);
			m_pEditScene = SceneManager::LoadFromFile(filePath);


			createEditorOnly();

			m_pActiveProject->SetOpenedSceneName(filePath.stem().string());
			m_bProjectChanged = true;
			};

		if (m_pEditScene->IsDirty())
		{
			activateSaveChanges(OnOpenScene);
		}
		else
		{
			OnOpenScene();
		}
	}

	void Editor::SaveScene()
	{
		if (m_pEditScene)
		{
			//SceneSerializer serializer(m_pEditScene);
			//serializer.Serialize(GetActiveSceneFilePath());
			SceneManager::SaveToFile(GetActiveSceneFilePath());
		}
	}

	void Editor::SaveSceneAs()
	{
		if (m_pEditScene)
		{
			FileUtils::SaveAs(
				GetActiveSceneFilePath(),
				"Dive Scene (*.dive)\0*.dive\0",
				//GEngine->GetWindowHandle()
				Window::GetWindowHandle()
			);
		}
	}

	void Editor::NewProject()
	{
		if (m_bProjectChanged || m_pEditScene->IsDirty())
		{
			activateSaveChanges(nullptr);
			m_bShowNewProjectPopup = true;
			m_bProjectChanged = false;
		}
		else
		{
			m_bShowNewProjectPopup = true;
			m_bProjectChanged = false;
		}
	}

	void Editor::OpenProject()
	{
		auto OnOpenProject = [this]() {
			auto filePath = FileUtils::OpenFile(
				"Dive Project (*.dive)\0*.dive\0",
				Window::GetWindowHandle()
			);

			if (filePath.empty() || filePath.stem() == m_pActiveProject->GetName())
				return;

			//SceneManager::UnloadScene();
			SceneManager::UnloadScene();
			DV_DELETE(m_pActiveProject);

			m_pActiveProject = Project::LoadFromFile(filePath);

			if (!m_pActiveProject->GetOpenedSceneName().empty())
			{
				std::filesystem::path worldFilePath = m_pActiveProject->GetScenesPath() / (m_pActiveProject->GetOpenedSceneName() + ".world");

				//m_pEditScene = new Scene;
				//SceneSerializer serializer(m_pEditScene);
				//serializer.Deserialize(worldFilePath);
				m_pEditScene = SceneManager::LoadFromFile(worldFilePath);

				createEditorOnly();
			}

			};

		if (m_bProjectChanged || m_pEditScene->IsDirty())
		{
			activateSaveChanges(OnOpenProject);
			m_bProjectChanged = false;
		}
		else
		{
			OnOpenProject();
		}
	}

	void Editor::SaveProject()
	{
		if (m_pActiveProject && m_bProjectChanged)
		{
			Project::SaveToFile(m_pActiveProject);
			m_bProjectChanged = false;
		}
	}

	void Editor::Exit()
	{
		auto OnExit = [this]() {
			SceneManager::UnloadScene();
			DV_DELETE(m_pActiveProject);
			DV_FIRE_EVENT(eEventType::Exit);
			};

		if (m_bProjectChanged || m_pEditScene->IsDirty())
		{
			activateSaveChanges(OnExit);
			m_bProjectChanged = false;
		}
		else
		{
			OnExit();
		}
	}

	const std::filesystem::path Editor::GetActiveSceneFilePath() const
	{
		if(!m_pActiveProject || !m_pEditScene)
			return std::filesystem::path();

		return m_pActiveProject->GetScenesPath() / (m_pEditScene->GetName() + ".dive");
	}

	const std::filesystem::path Editor::GetProjectDir() const
	{
		return m_pActiveProject ? m_pActiveProject->GetFilePath().parent_path() : std::filesystem::path();
	}
}