#include "Editor.h"
#include "Panels/WorldView.h"
#include "Panels/GameView.h"
#include "Panels/HierarchyView.h"
#include "Panels/InspectorView.h"
#include "Panels/AssetView.h"
#include "Project/Project.h"
#include "Project/ProjectSerializer.h"
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK EditorMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

	switch (msg)
	{
	case WM_SIZE:
	{
		auto data = std::make_unique<std::pair<uint32_t, uint32_t>>(LOWORD(lParam), HIWORD(lParam));
		DV_FIRE_EVENT_DATA(Dive::eEventType::WindowResized, (void*)data.get());
		return 0;
	}
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

namespace Dive
{
	Editor::Editor()
		: m_bProjectLoaded(false)
	{
		// 로그 매니져 설정
		Dive::LogManager::SetFilename("dive_editor.log");

		// 엔진 초기화 및 윈도우 설정
		GEngine->Initialize(::GetModuleHandle(nullptr), 1280, 720, L"Dive_Editor");
		GEngine->GetWindow()->ChangeProc((LONG_PTR)EditorMessageHandler);
		GEngine->GetWindow()->ShowWindow(SW_MAXIMIZE);

		// ImGui 초기화
		{
			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();

			ImGuiStyle& style = ImGui::GetStyle();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				style.WindowRounding = 0.0f;
				style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			}

			// Setup Platform/Renderer backends
			ImGui_ImplWin32_Init(GEngine->GetWindowHandle());
			ImGui_ImplDX11_Init(GEngine->GetDevice(), GEngine->GetDeviceContext());

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

			// 패널 생성
			m_Panels[static_cast<size_t>(ePanelTypes::WorldView)] = std::make_unique<WorldView>(this);
			m_Panels[static_cast<size_t>(ePanelTypes::GameView)] = std::make_unique<GameView>(this);
			m_Panels[static_cast<size_t>(ePanelTypes::HierarchyView)] = std::make_unique<HierarchyView>(this);
			m_Panels[static_cast<size_t>(ePanelTypes::InspectorView)] = std::make_unique<InspectorView>(this);
			m_Panels[static_cast<size_t>(ePanelTypes::AssetView)] = std::make_unique<AssetView>(this);
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
			drawMenubar();

			for (auto& pPanel : m_Panels)
			{
				pPanel->Draw();
			}
		}

		// End UI
		{
			ImGui::End();

			auto* pDefaultRenderTargetView = GEngine->GetGraphics()->GetBackbufferView();
			const float clear_color_with_alpha[4]{ 0.1f, 0.1f, 0.1f, 0.0f };
			GEngine->GetDeviceContext()->OMSetRenderTargets(1, &pDefaultRenderTargetView, NULL);
			GEngine->GetDeviceContext()->ClearRenderTargetView(pDefaultRenderTargetView, clear_color_with_alpha);

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

	bool Editor::CreateProject(const std::filesystem::path& path)
	{
		if (std::filesystem::exists(path))
		{
			DV_LOG(Editor, err, "이미 존재하는 경로({:s})에 새로운 프로젝트를 생성할 수 없습니다.", path.string());
			return false;
		}

		m_ProjectSettings.path = path.string();
		std::filesystem::create_directories(path);

		m_ProjectSettings.name = path.filename().string();

		// assets
		m_ProjectSettings.materialsPath = path.string() + "\\Assets\\Materials";
		std::filesystem::create_directories(m_ProjectSettings.materialsPath);
		m_ProjectSettings.modelsPath = path.string() + "\\Assets\\Models";
		std::filesystem::create_directories(m_ProjectSettings.modelsPath);
		m_ProjectSettings.texturesPath = path.string() + "\\Assets\\Textures";
		std::filesystem::create_directories(m_ProjectSettings.texturesPath);
		m_ProjectSettings.worldsPath = path.string() + "\\Assets\\Worlds";
		std::filesystem::create_directories(m_ProjectSettings.worldsPath);

		// file
		YAML::Node config;
		config["Name"] = m_ProjectSettings.name;
		config["Path"] = m_ProjectSettings.path;
		config["MaterialsPath"] = m_ProjectSettings.materialsPath;
		config["ModelsPath"] = m_ProjectSettings.modelsPath;
		config["TexturesPath"] = m_ProjectSettings.texturesPath;
		config["WorldsPath"] = m_ProjectSettings.worldsPath;

		std::ofstream fout(m_ProjectSettings.name + ".proj");
		fout << config;

		m_bProjectLoaded = true;

		SetTitle(m_ProjectSettings.name);

		return true;
	}

	bool Editor::OpenProject(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			DV_LOG(Editor, err, "파일({:s})이 존재하지 않습니다.", path.string());
			return false;
		}

		YAML::Node config = YAML::LoadFile(path.string());
		m_ProjectSettings.name = config["Name"].as<std::string>();
		m_ProjectSettings.path = config["Path"].as<std::string>();
		m_ProjectSettings.materialsPath = config["MaterialsPath"].as<std::string>();
		m_ProjectSettings.modelsPath = config["ModelsPath"].as<std::string>();
		m_ProjectSettings.texturesPath = config["TexturesPath"].as<std::string>();
		m_ProjectSettings.worldsPath = config["WorldsPath"].as<std::string>();

		m_bProjectLoaded = true;

		SetTitle(m_ProjectSettings.name);

		return true;
	}

	void Editor::CloseProject()
	{
		GEngine->CloseWorld();
		m_bProjectLoaded = false;

		SetTitle("");
	}

	void Editor::SetTitle(const std::string& projectName)
	{
		std::wstring title = L"Dive_Editor";

		if (!projectName.empty())
		{
			title += L" - " + StringUtils::StringToWString(projectName);
		}

		GEngine->GetWindow()->SetTitle(title.c_str());
	}

	void Editor::drawMenubar()
	{
		ImGui::BeginMainMenuBar();

		// FILE
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("New"))
				{
					if (ImGui::MenuItem("Project"))
					{
						if (m_pActiveProject && m_pActiveProject->IsModified())
						{

						}
					
						//m_bShowNewProject = true;

						
					}
					if (ImGui::MenuItem("World", nullptr, nullptr, (m_pActiveProject != nullptr)))
					{
						if (m_pActiveWorld)
						{

						}

						//m_bShowNewWorld = true;
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Open"))
				{
					if (ImGui::MenuItem("Project"))
					{
						if (m_pActiveProject && m_pActiveProject->IsModified())
						{

						}

						//ImGuiFileDialog::Instance()->OpenDialog("ChooseProject", "Choose Project", ".proj");

					}
					if (ImGui::MenuItem("World", nullptr, nullptr, (m_pActiveProject != nullptr)))
					{
						if (m_pActiveWorld)
						{

						}

						//ImGuiFileDialog::Instance()->OpenDialog("ChooseWorld", "Choose World", ".world",
						//	{ m_pEditor->GetProjectWorldsPath() });
					}
					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Close World", nullptr ,nullptr, (m_pActiveWorld != nullptr)))
				{
					GEngine->CloseWorld();
				}
				if (ImGui::MenuItem("Close Project", nullptr, nullptr, (m_pActiveProject != nullptr)))
				{
					//m_pEditor->CloseProject();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save", nullptr, nullptr, (m_pActiveProject != nullptr)))
				{
					//GEngine->GetActiveWorld()->SaveToFile(m_pEditor->GetProjectWorldsPath());
				}

				if (ImGui::MenuItem("Save As...", nullptr, nullptr, (m_pActiveProject != nullptr)))
				{

				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					if (m_pActiveProject)
					{
						// 저장 여부 확인
					}

					DV_FIRE_EVENT(eEventType::Exit);
				}

				ImGui::EndMenu();
			}
		}

		// Game Object
		{
			if (ImGui::BeginMenu("GameObject"))
			{
				auto pActiveWorld = GEngine->GetActiveWorld();

				if (ImGui::MenuItem("Create Empty", nullptr, nullptr, pActiveWorld != nullptr))
				{
					auto pNewGameObject = pActiveWorld->CreateGameObject();
				}

				if (ImGui::BeginMenu("3D Object"))
				{
					if (ImGui::MenuItem("Cube"))
					{
						/*
						if (m_pActiveWorld)
						{
							// 유니티에선 스크립트 차원에서 함수가 존재한다.
							// GameObject의 CreatePrimitive였나...

							// 가장 기본적인 형태는
							// GameObject를 생성하고
							// MeshRenderable을 추가한 후
							// ResourceManager를 통해 Model을 Set하는 것이다.

							Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Cube.obj");
						}
						*/
					}

					if (ImGui::MenuItem("Sphere"))
					{
						/*
						if (m_pActiveWorld)
						{
							Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Sphere.obj");
						}
						*/
					}

					if (ImGui::MenuItem("Cylinder"))
					{
						/*
						if (m_pActiveWorld)
						{
							Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Cylinder.obj");
						}
						*/
					}

					if (ImGui::MenuItem("Cone"))
					{
						/*
						if (m_pActiveWorld)
						{
							Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Cone.obj");
						}
						*/
					}

					if (ImGui::MenuItem("Plane"))
					{
						/*
						if (m_pActiveWorld)
						{
							Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Plane.obj");
						}
						*/
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Light"))
				{
					if (ImGui::MenuItem("Directional"))
					{
						/*
						if (m_pActiveWorld)
						{
							auto lightObj = m_pActiveWorld->CreateGameObject("Directional_Light");
							lightObj->AddComponent<Dive::Transform>();
							auto lightCom = lightObj->AddComponent<Dive::Light>();
							lightCom->SetLightType(Dive::eLightType::Directional);
						}
						*/
					}

					if (ImGui::MenuItem("Point"))
					{
						/*
						if (m_pActiveWorld)
						{
							auto lightObj = m_pActiveWorld->CreateGameObject("Point_Light");
							lightObj->AddComponent<Dive::Transform>();
							auto lightCom = lightObj->AddComponent<Dive::Light>();
							lightCom->SetLightType(Dive::eLightType::Point);
						}
						*/
					}

					if (ImGui::MenuItem("Spot"))
					{
						/*
						if (m_pActiveWorld)
						{
							auto lightObj = m_pActiveWorld->CreateGameObject("Spot_Light");
							lightObj->AddComponent<Dive::Transform>();
							auto lightCom = lightObj->AddComponent<Dive::Light>();
							lightCom->SetLightType(Dive::eLightType::Spot);
						}
						*/
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Camera"))
				{
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}
		}

		// Component
		{
			if (ImGui::BeginMenu("Component"))
			{
				if (ImGui::MenuItem("SpriteRenderable"))
				{
					/*
					if (m_pActiveWorld)
					{
						auto pSelected = m_pEditor->GetHierarchy()->GetSeletecedObject();
						if (pSelected)
						{
							if (!pSelected->HasComponent<Dive::SpriteRenderable>())
							{
								pSelected->AddComponent<Dive::SpriteRenderable>();
							}
						}
					}
					*/
				}

				if (ImGui::MenuItem("Light"))
				{
					/*
					if (m_pActiveWorld)
					{
						auto pSelected = m_pEditor->GetHierarchy()->GetSeletecedObject();
						if (pSelected)
						{
							if (!pSelected->HasComponent<Dive::Light>())
							{
								pSelected->AddComponent<Dive::Light>();
							}
						}
					}
					*/
				}

				ImGui::EndMenu();
			}
		}

		// Views
		{
			if (ImGui::BeginMenu("View"))
			{
				static bool showView = true;

				showView = m_Panels[static_cast<size_t>(ePanelTypes::WorldView)]->IsVisible();
				ImGui::Checkbox("WorldView", &showView);
				m_Panels[static_cast<size_t>(ePanelTypes::WorldView)]->SetVisible(showView);

				showView = m_Panels[static_cast<size_t>(ePanelTypes::GameView)]->IsVisible();
				ImGui::Checkbox("GameView", &showView);
				m_Panels[static_cast<size_t>(ePanelTypes::GameView)]->SetVisible(showView);

				showView = m_Panels[static_cast<size_t>(ePanelTypes::HierarchyView)]->IsVisible();
				ImGui::Checkbox("HierarchyView", &showView);
				m_Panels[static_cast<size_t>(ePanelTypes::HierarchyView)]->SetVisible(showView);

				showView = m_Panels[static_cast<size_t>(ePanelTypes::InspectorView)]->IsVisible();
				ImGui::Checkbox("InspectorView", &showView);
				m_Panels[static_cast<size_t>(ePanelTypes::InspectorView)]->SetVisible(showView);

				showView = m_Panels[static_cast<size_t>(ePanelTypes::AssetView)]->IsVisible();
				ImGui::Checkbox("AssetView", &showView);
				m_Panels[static_cast<size_t>(ePanelTypes::AssetView)]->SetVisible(showView);


				ImGui::EndMenu();
			}
		}

		// Help
		{
			if (ImGui::BeginMenu("Help"))
			{
				ImGui::EndMenu();
			}
		}

		ImGui::EndMainMenuBar();
	}
}