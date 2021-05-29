#include "Editor.h"
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_internal.h"
#include "External/ImGui/imgui_impl_win32.h"
#include "External/ImGui/imgui_impl_dx11.h"
#include "MenuBar.h"
#include "World.h"
#include "Viewport.h"
#include "Console.h"
#include "Assets.h"
#include "Component.h"

namespace Editor
{
	Editor::Editor()
	{
		m_iniFilePath = Dive::FileSystemHelper::GetWorkingDirectory() + "editor.ini";
		m_bMaximize = false;
	}

	Editor::~Editor()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	bool Editor::Initialize()
	{
		initialized_ini();

		if (!Dive::Runtime::Initialize())
			return false;

		initialize_ImGui();

		ShowWindow(m_hWnd, m_bMaximize ? SW_MAXIMIZE : SW_SHOWDEFAULT);
		UpdateWindow(m_hWnd);

		APP_TRACE("Editor의 초기화에 성공하였습니다.");

		return true;
	}

	void Editor::Render()
	{
		auto graphicsDevice = Dive::Renderer::GetInstance().GetGraphicsDevice();
		graphicsDevice->PresentBegin();

		// ImGui에서 그리는 부분
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();


			begin_ImGui();

			// 이 곳에서 Widget을 그릴 것이다.
			for (auto pWidget : m_widgets)
			{
				pWidget->Tick();
			}

			end_ImGui();

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			// Update and Render additional Platform Windows
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}

		// 이건 엔진에서 그리는 부분
		// 에디터에서는 RenderPath3D를 수정해야 한다.
		// Debug용 Render가 추가되고,
		// World이 다른 RenderTarget에 그려져야 하기 때문이다.
		if (m_activePath)
		{
			m_activePath->Render();
		}

		Compose();
		graphicsDevice->PresentEnd();
	}

	void Editor::ResizeWindow(unsigned int width, unsigned int height)
	{
		Dive::IniHelper ini(m_iniFilePath);
		ini["Window"]["Width"] = width;
		ini["Window"]["Height"] = height;
		ini["Window"]["bMaximize"] = IsZoomed(m_hWnd) ? true : false;
	}

	void Editor::initialized_ini()
	{
		Dive::IniHelper ini(m_iniFilePath);
		m_bMaximize = ini["Window"]["bMaximize"] << false;
		int width = ini["Window"]["Width"] << 800;
		int height = ini["Window"]["Height"] << 600;
		int posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		if (!Dive::FileSystemHelper::FileExists(m_iniFilePath))
		{
			ini["Window"]["Width"] = width;
			ini["Window"]["Height"] = height;
			ini["Window"]["bMaximize"] = m_bMaximize;
		}

		RECT rcClient = { 0, 0, width, height };
		AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);
		SetWindowPos(m_hWnd, nullptr, posX, posY, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top), 0);
	}

	void Editor::initialize_ImGui()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;
		//io.ConfigViewportsNoDefaultParent = true;
		//io.ConfigDockingAlwaysTabBar = true;
		//io.ConfigDockingTransparentPayload = true;
		//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
		//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplWin32_Init(m_hWnd);

		auto pDevice = Dive::Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		auto pImmediateContext = Dive::Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		ImGui_ImplDX11_Init(pDevice, pImmediateContext);

		io.Fonts->AddFontFromFileTTF("../Assets/Fonts/NanumGothic.ttf", 14);

		// Widget 생성
		m_widgets.emplace_back(std::make_shared<MenuBar>(this));
		m_widgets.emplace_back(std::make_shared<World>(this));
		m_widgets.emplace_back(std::make_shared<Viewport>(this));
		m_widgets.emplace_back(std::make_shared<Component>(this));
		m_widgets.emplace_back(std::make_shared<Assets>(this));
		//m_widgets.emplace_back(std::make_shared<Console>(this));
	}

	void Editor::begin_ImGui()
	{
		// 전체 화면을 커버할 위젯을 만드는 것 같다.
		const auto window_flags =
			ImGuiWindowFlags_MenuBar |				
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;

		float offset_y = 8.0f;// _editor::widget_menu_bar ? (_editor::widget_menu_bar->GetHeight() + _editor::widget_menu_bar->GetPadding()) : 0;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + offset_y));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - offset_y));
		ImGui::SetNextWindowViewport(viewport->ID);

		// Set window style
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowBgAlpha(0.0f);

		const char* dockSpaceName = "DiveEditor";

		bool open = true;
		m_editor_begun = ImGui::Begin(dockSpaceName, &open, window_flags);
		ImGui::PopStyleVar(3);

		// 크기를 설정해야 한다.
		// 미리 docking 해놓았던데 그건 패스해도 될 것 같다.
		// Begin dock space
		if (m_editor_begun)
		{
			// Dock space
			const auto window_id = ImGui::GetID(dockSpaceName);
			if (!ImGui::DockBuilderGetNode(window_id))
			{
				// Reset current docking state
				ImGui::DockBuilderRemoveNode(window_id);
				ImGui::DockBuilderAddNode(window_id, ImGuiDockNodeFlags_None);
				ImGui::DockBuilderSetNodeSize(window_id, ImGui::GetMainViewport()->Size);

				// DockBuilderSplitNode(ImGuiID node_id, ImGuiDir split_dir, float size_ratio_for_node_at_dir, ImGuiID* out_id_dir, ImGuiID* out_id_other);
				ImGuiID dock_main_id = window_id;
				ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.3f, nullptr, &dock_main_id);
				const ImGuiID dock_right_right_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Right, 0.5f, nullptr, &dock_right_id);
				ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, nullptr, &dock_main_id);

				// Dock windows
				// 이름과 연관이 있는 것 같다???
				ImGui::DockBuilderDockWindow("Viewport", dock_main_id);
				ImGui::DockBuilderDockWindow("World", dock_right_id);
				ImGui::DockBuilderDockWindow("Component", dock_right_right_id);
				ImGui::DockBuilderDockWindow("Assets", dock_down_id);

				ImGui::DockBuilderFinish(dock_main_id);
			}

			ImGui::DockSpace(window_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		}
	}
	
	void Editor::end_ImGui()
	{
		if(m_editor_begun)
			ImGui::End();
	}
}