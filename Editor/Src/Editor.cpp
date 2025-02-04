#include "Editor.h"
#include "Panels/WorldView.h"
#include "Panels/GameView.h"
#include "Panels/HierarchyView.h"
#include "Panels/InspectorView.h"
#include "Panels/ProjectView.h"
#include "Project/Project.h"
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

static Dive::Editor* s_pEditor = nullptr;

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
	{
		if (s_pEditor)
			s_pEditor->Exit();
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

namespace Dive
{
	static uint16_t s_WorldCounter = 0;

	Editor::Editor()
		: m_pActiveProject(nullptr)
		, m_bProjectChanged(false)
		, m_bWorldChanged(false)
		, m_bShowSaveChangesPopup(false)
		, m_bShowNewProjectPopup(false)
		, m_bShowNewWorldPopup(false)
	{
		s_pEditor = this;

		// �α� �Ŵ��� ����
		Dive::LogManager::SetFilename("dive_editor.log");

		// ���� �ʱ�ȭ �� ������ ����
		GEngine->Initialize(::GetModuleHandle(nullptr), 1600, 900, L"Dive Editor");
		GEngine->GetWindow()->ChangeProc((LONG_PTR)EditorMessageHandler);
		//GEngine->GetWindow()->ShowWindow(SW_MAXIMIZE);

		// ImGui �ʱ�ȭ
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

			// ��Ÿ�� ����
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

			// �г� ����
			m_Panels.push_back(std::make_unique<WorldView>(this));
			m_Panels.push_back(std::make_unique<GameView>(this));
			m_Panels.push_back(std::make_unique<HierarchyView>(this));
			m_Panels.push_back(std::make_unique<InspectorView>(this));
			m_Panels.push_back(std::make_unique<ProjectView>(this));
		}

		// �ӽ� ������Ʈ �� ���� ����
		{
			if (std::filesystem::exists("New Project"))
			{
				m_pActiveProject = Project::LoadFromFile("New Project/New Project.proj");
				if (m_pActiveProject)
				{
					auto& openedWorldName = m_pActiveProject->GetOpenedWorldName();
					std::filesystem::path worldFilePath;
					if (!openedWorldName.empty())
						worldFilePath = m_pActiveProject->GetWorldsPath() / (openedWorldName + ".world");
					else
						worldFilePath = m_pActiveProject->GetWorldsPath() / "New World.world";

					m_pActiveWorld = new World(openedWorldName);
					WorldSerializer serializer(m_pActiveWorld);
					serializer.Deserialize(worldFilePath);
				}
			}
			else
			{
				m_pActiveProject = Project::New(std::filesystem::current_path() / "New Project/New Project.proj");
				if (m_pActiveProject)
				{
					Project::SaveToFile(m_pActiveProject);

					m_pActiveWorld = new World("New World");
					WorldSerializer serializer(m_pActiveWorld);
					serializer.Serialize(GetActiveWorldFilePath());
				}
			}

			updateTitle();
			s_WorldCounter++;
		}

		DV_SUBSCRIBE_EVENT(Dive::eEventType::PostRender, DV_EVENT_HANDLER(OnPostRender));
	}

	Editor::~Editor()
	{
		s_pEditor = nullptr;

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		GEngine->Shutdown();
	}

	int Editor::Run()
	{
		// Engine���� �и��� ��
		// ��ġ�� ����� �ָ�������.
		if (m_pActiveWorld)
			m_pActiveWorld->Tick();

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
			style.WindowMinSize.x = 200.0f;	// dock space������ �ּ� ũ��...
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

			for (auto& pPanel : m_Panels)
				pPanel->Draw();
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

	void Editor::updateTitle()
	{
		GEngine->GetWindow()->SetTitle(
			m_pActiveProject ? m_pActiveProject->GetNameW().c_str() : L"Dive Editor"
		);
	}

	void Editor::drawMainManubar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			// FILE
			if (ImGui::BeginMenu(u8"����"))
			{
				if (ImGui::MenuItem(u8"���ο� ����", nullptr, nullptr, m_pActiveProject))
				{
					NewWorld();
				}
				if (ImGui::MenuItem(u8"���� ����", nullptr, nullptr, m_pActiveProject))
				{
					OpenWorld();
				}
				
				ImGui::Separator();
				
				if (ImGui::MenuItem(u8"���� ����", nullptr, nullptr, m_pActiveWorld && m_bWorldChanged))
				{
					SaveWorld();
				}
				if (ImGui::MenuItem(u8"�ٸ� �̸����� ���� ����", nullptr, nullptr, m_pActiveWorld))
				{
					SaveWorldAs();
				}

				ImGui::Separator();
				
				if (ImGui::MenuItem(u8"���ο� ������Ʈ"))
				{
					NewProject();
				}
				if (ImGui::MenuItem(u8"������Ʈ ����"))
				{
					OpenProject();
				}
				if (ImGui::MenuItem(u8"������Ʈ ����", nullptr, nullptr, m_pActiveProject && m_bProjectChanged))
				{
					SaveProject();
				}

				ImGui::Separator();

				if (ImGui::MenuItem(u8"������"))
				{
					Exit();
				}
				ImGui::EndMenu();
			}

			// Game Object
			if (ImGui::BeginMenu(u8"���� ������Ʈ"))
			{
				if (ImGui::MenuItem(u8"�� ���� ������Ʈ", nullptr, nullptr, (m_pActiveWorld != nullptr)))
				{
					auto pNewGameObject = m_pActiveWorld->CreateGameObject();
					m_bWorldChanged = true;
				}

				if (ImGui::BeginMenu(u8"3D ������Ʈ", (m_pActiveWorld != nullptr)))
				{
					if (ImGui::MenuItem(u8"����"))
					{
						
						m_bWorldChanged = true;
					}

					if (ImGui::MenuItem(u8"��"))
					{
						
						m_bWorldChanged = true;
					}

					if (ImGui::MenuItem(u8"���"))
					{
						
						m_bWorldChanged = true;
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu(u8"����Ʈ", (m_pActiveWorld != nullptr)))
				{
					if (ImGui::MenuItem(u8"�𷺼ų� ����Ʈ"))
					{
						m_bWorldChanged = true;
					}

					if (ImGui::MenuItem(u8"����Ʈ ����Ʈ"))
					{
						m_bWorldChanged = true;
					}

					if (ImGui::MenuItem(u8"����Ʈ ����Ʈ"))
					{
						m_bWorldChanged = true;
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu(u8"ī�޶�", (m_pActiveWorld != nullptr)))
				{
					if (ImGui::MenuItem(u8"���� ����"))
					{
						m_bWorldChanged = true;
					}

					if (ImGui::MenuItem(u8"���� ����"))
					{
						m_bWorldChanged = true;
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			// Component
			// SelectedGameObject�� ������ �� Ȱ��ȭ
			if (ImGui::BeginMenu(u8"������Ʈ"))
			{
				if (ImGui::MenuItem(u8"��������", nullptr, nullptr, (m_pSelectedGameObject != nullptr)))
				{
					m_bWorldChanged = true;
				}

				if (ImGui::MenuItem(u8"����Ʈ", nullptr, nullptr, (m_pSelectedGameObject != nullptr)))
				{
					m_bWorldChanged = true;
				}

				ImGui::EndMenu();
			}

			// Views
			if (ImGui::BeginMenu(u8"â"))
			{
				static bool showView = true;

				/*
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

				showView = m_Panels[static_cast<size_t>(ePanelTypes::ProjectView)]->IsVisible();
				ImGui::Checkbox("ProjectView", &showView);
				m_Panels[static_cast<size_t>(ePanelTypes::ProjectView)]->SetVisible(showView);
				*/

				ImGui::EndMenu();
			}

			// Help
			if (ImGui::BeginMenu(u8"����"))
			{
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
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
			ImGui::Text(u8"��������� �����մϴ�. �����Ͻðڽ��ϱ�?");
			// ���� ������ �ִ� ������ ǥ��
			if (ImGui::Button(u8"����", ImVec2(120, 0)))
			{
				// �̰� �̺��ϴ�. �ټ��� ���尡 �����Ǿ��� �� �ִ�.
				// => �׷��ٸ� �ᱹ ���庰�� �������θ� �Ǵ��ؾ� �Ѵٴ� �ǵ�?
				if (m_pActiveWorld && m_bWorldChanged)
				{
					WorldSerializer serializer(m_pActiveWorld);
					serializer.Serialize(GetActiveWorldFilePath());
					m_pActiveProject->SetOpenedWorldName(m_pActiveWorld->GetName());
				}
				if(m_bProjectChanged)
					Project::SaveToFile(m_pActiveProject);
				if (m_OnConfirm) m_OnConfirm();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"���� ����", ImVec2(120, 0)))
			{
				if (m_OnConfirm) m_OnConfirm();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"���", ImVec2(120, 0)))
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
			ImGui::Text(u8"���ο� ������Ʈ");
			ImGui::PopFont();

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			// ���� ������Ʈ �̸�
			static std::string projectName = "New_Project";
			char projectNameBuffer[256];
			strncpy_s(projectNameBuffer, projectName.c_str(), sizeof(projectNameBuffer));
			ImGui::Text(u8"������Ʈ �̸�");
			ImGui::PushItemWidth(400);
			if (ImGui::InputText("##project_name", projectNameBuffer, sizeof(projectNameBuffer)))
			{
				projectName = std::string(projectNameBuffer);
			}
			ImGui::PopItemWidth();

			ImGui::Spacing();

			// ���� ������Ʈ ��ġ
			static std::filesystem::path projectPath = std::filesystem::current_path();
			char projectPathBuffer[512];
			strncpy_s(projectPathBuffer, projectPath.string().c_str(), sizeof(projectPathBuffer));
			ImGui::Text(u8"��ġ");
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

			if (ImGui::Button(u8"���", ImVec2(100, 30)))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"�����", ImVec2(100, 30)))
			{
				// �ش� ���丮�� �̹� �����Ѵٸ� ��� ��ȭ���ڸ� ����� �Ѵ�.
				if (std::filesystem::exists(projectPath / projectName))
				{
					ImGui::OpenPopup("Exist same path");
					//::MessageBox(nullptr, L"������ ������Ʈ ���丮�� �̹� �����Ͽ� ������ �� �����ϴ�.", L"���", MB_OK);
				}
				else
				{
					// ���ο� ������Ʈ ����
					m_pActiveProject = Project::New(projectPath / projectName / (projectName + ".proj"));
					updateTitle();

					// ���ο� ���� ����
					m_pActiveWorld = new World("New World");
					WorldSerializer serializer(m_pActiveWorld);
					serializer.Serialize(GetActiveWorldFilePath());

					m_pActiveProject->SetOpenedWorldName(m_pActiveWorld->GetName());

					ImGui::CloseCurrentPopup();
				}
			}

			if (ImGui::BeginPopupModal("Exist same path", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text(u8"�̹� �����ϴ� ������Ʈ ���丮�� ������ �� �����ϴ�.");
				if (ImGui::Button(u8"Ȯ��"))
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

	void Editor::NewWorld()
	{
		auto OnCreateNewWorld = [this]() {
			DV_DELETE(m_pActiveWorld);
			
			m_pActiveWorld = new World("New World " + std::to_string(s_WorldCounter++));	// �̰ɷ� ������ �̸��� Ȯ���� �� ����.
				
			WorldSerializer serializer(m_pActiveWorld);
			serializer.Serialize(GetActiveWorldFilePath());

			m_pActiveProject->SetOpenedWorldName(m_pActiveWorld->GetName());
			m_bProjectChanged = true;
			};

		if (m_bWorldChanged)
		{
			activateSaveChanges(OnCreateNewWorld);
			m_bWorldChanged = false;
		}
		else
		{
			OnCreateNewWorld();
		}
	}

	void Editor::OpenWorld()
	{
		auto OnOpenWorld = [this]() {
			DV_DELETE(m_pActiveWorld);

			auto filePath = FileUtils::OpenFile(
				"Dive World (*.world)\0*.world\0",
				GEngine->GetWindowHandle(),
				m_pActiveProject->GetWorldsPath().string().c_str()
			);
			if (filePath.empty()) return;

			m_pActiveWorld = new World;
			WorldSerializer serializer(m_pActiveWorld);
			serializer.Deserialize(filePath);

			m_pActiveProject->SetOpenedWorldName(filePath.stem().string());
			m_bProjectChanged = true;
			};

		if (m_bWorldChanged)
		{
			activateSaveChanges(OnOpenWorld);
			m_bWorldChanged = false;
		}
		else
		{
			OnOpenWorld();
		}
	}

	void Editor::SaveWorld()
	{
		if (m_pActiveWorld)
		{
			WorldSerializer serializer(m_pActiveWorld);
			serializer.Serialize(GetActiveWorldFilePath());
			m_bWorldChanged = false;
		}
	}

	void Editor::SaveWorldAs()
	{
		if (m_pActiveWorld)
		{
			FileUtils::SaveAs(
				GetActiveWorldFilePath(),
				"Dive World (*.world)\0*.world\0",
				GEngine->GetWindowHandle()
			);
		}
	}

	void Editor::NewProject()
	{
		if (m_bProjectChanged || m_bWorldChanged)
		{
			activateSaveChanges(nullptr);
			m_bShowNewProjectPopup = true;
			m_bProjectChanged = false;
			m_bWorldChanged = false;
		}
		else
		{
			m_bShowNewProjectPopup = true;
			m_bProjectChanged = false;
			m_bWorldChanged = false;
		}
	}

	void Editor::OpenProject()
	{
		auto OnOpenProject = [this]() {
			auto filePath = FileUtils::OpenFile(
				"Dive Project (*.proj)\0*.proj\0",
				GEngine->GetWindowHandle()
			);

			if (filePath.empty() || filePath.stem() == m_pActiveProject->GetName())
				return;

			DV_DELETE(m_pActiveWorld);
			DV_DELETE(m_pActiveProject);

			m_pActiveProject = Project::LoadFromFile(filePath);
			updateTitle();

			if (!m_pActiveProject->GetOpenedWorldName().empty())
			{
				std::filesystem::path worldFilePath = m_pActiveProject->GetWorldsPath() / (m_pActiveProject->GetOpenedWorldName() + ".world");

				m_pActiveWorld = new World;
				WorldSerializer serializer(m_pActiveWorld);
				serializer.Deserialize(worldFilePath);
			}

			};

		if (m_bProjectChanged || m_bWorldChanged)
		{
			activateSaveChanges(OnOpenProject);
			m_bProjectChanged = false;
			m_bWorldChanged = false;
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
			DV_DELETE(m_pActiveWorld);
			DV_DELETE(m_pActiveProject);
			DV_FIRE_EVENT(eEventType::Exit);
			};

		if (m_bProjectChanged || m_bWorldChanged)
		{
			activateSaveChanges(OnExit);
			m_bProjectChanged = false;
			m_bWorldChanged = false;
		}
		else
		{
			OnExit();
		}
	}

	const std::filesystem::path Editor::GetActiveWorldFilePath() const
	{
		if(!m_pActiveProject || !m_pActiveWorld)
			return std::filesystem::path();

		return m_pActiveProject->GetWorldsPath() / (m_pActiveWorld->GetName() + ".world");
	}

	const std::filesystem::path Editor::GetProjectDir() const
	{
		return m_pActiveProject ? m_pActiveProject->GetFilePath().parent_path() : std::filesystem::path();
	}
}