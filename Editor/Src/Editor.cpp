#include "Editor.h"
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include "Panels/Menubar.h"
#include "Panels/WorldView.h"
#include "Panels/GameView.h"
#include "Panels/HierarchyView.h"
#include "Panels/InspectorView.h"
#include "Panels/AssetView.h"

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
		for (auto& font : m_Fonts)
		{
			font = nullptr;
		}
	}

	int Editor::Run()
	{
		// 엔진 설정 및 초기화
		{
			Dive::LogManager::SetFilename("dive_editor.log");

			if (!GEngine->Initialize(::GetModuleHandle(nullptr), 1280, 720, L"Dive_Editor"))
				return 1;

			if (!::SetWindowLongPtr(GEngine->GetWindowHandle(), GWLP_WNDPROC, (LONG_PTR)EditorMessageHandler))
			{
				DV_LOG(Window, err, "윈도우 프로시져 함수 변경 실패");
				return 1;
			}

			GEngine->GetWindow()->ShowWindow(SW_MAXIMIZE);

		}

		// 에디터 설정 및 초기화
		{
			initializeImGui();
			DV_SUBSCRIBE_EVENT(Dive::eEventType::PostRender, DV_EVENT_HANDLER(OnPostRender));
		}

		// 실행
		GEngine->Tick();

		// 에디터 셧다운
		{
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}

		// 엔진 셧다운
		GEngine->Shutdown();

		return 0;
	}

	void Editor::OnPostRender()
	{
		// 추후엔 백버퍼뷰를 직접 바인딩해야 한다.
		renderImGui();
	}

	Panel& Editor::GetPanel(ePanelTypes type)
	{
		return *m_Panels[static_cast<size_t>(type)].get();
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

	void Editor::initializeImGui()
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

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(GEngine->GetWindowHandle());
		ImGui_ImplDX11_Init(GEngine->GetDevice(), GEngine->GetDeviceContext());

		// dark theme
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

		// load fonts
		ImFontConfig config;
		config.SizePixels = 19.0f;
		m_Fonts[static_cast<size_t>(eFontTypes::Default)] = io.Fonts->AddFontDefault(&config);
		float fontSize = 19.0f;
		m_Fonts[static_cast<size_t>(eFontTypes::Normal)] = io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/NanumBarunGothic.ttf", fontSize, nullptr, io.Fonts->GetGlyphRangesKorean());
		m_Fonts[static_cast<size_t>(eFontTypes::Bold)] = io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/NanumBarunGothicBold.ttf", fontSize, nullptr, io.Fonts->GetGlyphRangesKorean());
		fontSize = 26.0f;
		m_Fonts[static_cast<size_t>(eFontTypes::Large)] = io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/NanumBarunGothic.ttf", fontSize, nullptr, io.Fonts->GetGlyphRangesKorean());
		m_Fonts[static_cast<size_t>(eFontTypes::Large_Bold)] = io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/NanumBarunGothicBold.ttf", fontSize, nullptr, io.Fonts->GetGlyphRangesKorean());
		
		// create panels
		m_Panels[static_cast<size_t>(ePanelTypes::Menubar)] = std::make_unique<Menubar>(this);
		m_Panels[static_cast<size_t>(ePanelTypes::WorldView)] = std::make_unique<WorldView>(this);
		m_Panels[static_cast<size_t>(ePanelTypes::GameView)] = std::make_unique<GameView>(this);
		m_Panels[static_cast<size_t>(ePanelTypes::HierarchyView)] = std::make_unique<HierarchyView>(this);
		m_Panels[static_cast<size_t>(ePanelTypes::InspectorView)] = std::make_unique<InspectorView>(this);
		m_Panels[static_cast<size_t>(ePanelTypes::AssetView)] = std::make_unique<AssetView>(this);

		DV_LOG(Editor, trace, "ImGui 초기화를 수행하였습니다.");
	}

	void Editor::renderImGui()
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

		// Render Panels
		for (auto& panel : m_Panels)
		{
			if (!panel)
			{
				DV_LOG(Editor, err, "panel이 존재하지 않습니다.");
			}

			panel->Tick();
		}

		ImGui::End();

		// 일단 임시다.
		auto* pDefaultRenderTargetView = GEngine->GetGraphics()->GetBackbufferView();
		const float clear_color_with_alpha[4]{ 0.1f, 0.1f, 0.1f, 0.0f };
		GEngine->GetDeviceContext()->OMSetRenderTargets(1, &pDefaultRenderTargetView, NULL);
		GEngine->GetDeviceContext()->ClearRenderTargetView(pDefaultRenderTargetView, clear_color_with_alpha);

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		//ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
}