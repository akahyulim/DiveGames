#include "Editor.h"
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_internal.h"
#include "External/ImGui/imgui_impl_win32.h"
#include "External/ImGui/imgui_impl_dx11.h"
#include "MenuBar.h"
#include "Hierarchy.h"
#include "Scene.h"
#include "Assets.h"
#include "Inspector.h"
#include <iostream>


namespace Editor
{
	Editor::Editor()
	{
		m_AppTitle = "DiveEditor";
	}

	Editor::~Editor()
	{
		if (ImGui::GetCurrentContext())
		{
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}
	}

	bool Editor::Initialize()
	{
		if (!dive::Runtime::Initialize())
			return false;

		// ImGui 초기화 및 Widget 생성
		initialize_ImGui();

		// RenderPath 적용
		ActivatePath(&m_RenderPathEditor);

		APP_TRACE("Editor::Initialize()");

		return true;
	}

	/*
	* ImGUI를 그려내기 위한 위치는 이 곳이 유일했다.
	* 현재 Path의 Compose를 비활성화 하였는데
	* 추후 어떻게 구현할 지 생각을 해야 한다.
	*/
	void Editor::Compose()
	{
//		if (m_pActivePath)
		{
//			m_pActivePath->Compose();
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		beginDockSpace();

		for (auto pWidget : m_Widgets)
		{
			pWidget->Tick();
		}

		endDockSpace();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void Editor::ModifyWindow(dive::eWindowModes mode, unsigned int width, unsigned int height, bool maximize)
	{
		unsigned int posX = 0;
		unsigned int posY = 0;

		LONG style = WS_OVERLAPPEDWINDOW;

		RECT rt = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
		AdjustWindowRect(&rt, style, 0);

		width = rt.right - rt.left;
		height = rt.bottom - rt.top;
			
		SetWindowLong(m_hWnd, GWL_STYLE, style);

		posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
		
		SetWindowPos(m_hWnd, NULL, posX, posY, width, height, 0);

		ShowWindow(m_hWnd, maximize ? SW_MAXIMIZE : SW_SHOWDEFAULT);
		UpdateWindow(m_hWnd);
	}

	void Editor::initialize_ImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigWindowsResizeFromEdges = true;
		io.ConfigViewportsNoTaskBarIcon = true;

		applyStyle();

		ImGui_ImplWin32_Init(m_hWnd);

		auto pDevice = dive::Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		auto pImmediateContext = dive::Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		ImGui_ImplDX11_Init(pDevice, pImmediateContext);

		// Widget 생성
		m_Widgets.emplace_back(std::make_shared<MenuBar>(this));
		m_Widgets.emplace_back(std::make_shared<Hierarchy>(this));
		m_Widgets.emplace_back(std::make_shared<Scene>(this));
		m_Widgets.emplace_back(std::make_shared<Inspector>(this));
		m_Widgets.emplace_back(std::make_shared<Assets>(this));
	}

	void Editor::beginDockSpace()
	{
		const auto window_flags =
			ImGuiWindowFlags_MenuBar |				
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;

		float offset_y = 8.0f;
		const ImGuiViewport* pViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(pViewport->Pos.x, pViewport->Pos.y + offset_y));
		ImGui::SetNextWindowSize(ImVec2(pViewport->Size.x, pViewport->Size.y - offset_y));
		ImGui::SetNextWindowViewport(pViewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowBgAlpha(0.0f);

		const char* dockSpaceName = "DiveEditor";

		bool open = true;
		m_bDockSpace = ImGui::Begin(dockSpaceName, &open, window_flags);
		ImGui::PopStyleVar(3);

		if (m_bDockSpace)
		{
			const auto window_id = ImGui::GetID(dockSpaceName);
			if (!ImGui::DockBuilderGetNode(window_id))
			{
				ImGui::DockBuilderRemoveNode(window_id);
				ImGui::DockBuilderAddNode(window_id, ImGuiDockNodeFlags_None);
				ImGui::DockBuilderSetNodeSize(window_id, ImGui::GetMainViewport()->Size);

				ImGuiID dock_main_id = window_id;
				ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
				const ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);
				ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, nullptr, &dock_main_id);

				ImGui::DockBuilderDockWindow("Scene", dock_main_id);
				ImGui::DockBuilderDockWindow("Hierarchy", dock_left_id);
				ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
				ImGui::DockBuilderDockWindow("Assets", dock_down_id);

				ImGui::DockBuilderFinish(dock_main_id);
			}

			ImGui::DockSpace(window_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		}
	}
	
	void Editor::endDockSpace()
	{
		if(m_bDockSpace)
			ImGui::End();
	}

	//==========================================================================================//
	// 1. 폰트 파일 존재 유무 확인이 필요하다.													//
	// 2. 모르는 변수가 많고 이름도 내 스타일이 아니다.											//
	// 3. Modal Popup 사용시 Transparency가 되지 않아 전부 주석처리 했다.						//
	//==========================================================================================//
	void Editor::applyStyle() const
	{
		/*
		// Color settings    
		const auto color_text = ImVec4(0.810f, 0.810f, 0.810f, 1.000f);
		const auto color_text_disabled = ImVec4(color_text.x, color_text.y, color_text.z, 0.5f);
		const auto color_interactive = ImVec4(0.338f, 0.338f, 0.338f, 1.000f);
		const auto color_interactive_hovered = ImVec4(0.450f, 0.450f, 0.450f, 1.000f);
		const auto color_interactive_clicked = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
		const auto color_background = ImVec4(50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1.0f);
		const auto color_background_content = ImVec4(35.0f / 255.0f, 35.0f / 255.0f, 35.0f / 255.0f, 1.0f);
		const auto color_shadow = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);

		// Use default dark style as a base
		ImGui::StyleColorsDark();

		// Colors
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = color_text;
		colors[ImGuiCol_TextDisabled] = color_text_disabled;
		colors[ImGuiCol_WindowBg] = color_background;             // Background of normal windows
		colors[ImGuiCol_ChildBg] = color_background;             // Background of child windows
		colors[ImGuiCol_PopupBg] = color_background;             // Background of popups, menus, tooltips windows
		colors[ImGuiCol_Border] = color_interactive;
		colors[ImGuiCol_BorderShadow] = color_shadow;
		colors[ImGuiCol_FrameBg] = color_background_content;     // Background of checkbox, radio button, plot, slider, text input
		colors[ImGuiCol_FrameBgHovered] = color_interactive;
		colors[ImGuiCol_FrameBgActive] = color_interactive_clicked;
		colors[ImGuiCol_TitleBg] = color_background_content;
		colors[ImGuiCol_TitleBgActive] = color_interactive;
		colors[ImGuiCol_TitleBgCollapsed] = color_background;
		colors[ImGuiCol_MenuBarBg] = color_background_content;
		colors[ImGuiCol_ScrollbarBg] = color_background_content;
		colors[ImGuiCol_ScrollbarGrab] = color_interactive;
		colors[ImGuiCol_ScrollbarGrabHovered] = color_interactive_hovered;
		colors[ImGuiCol_ScrollbarGrabActive] = color_interactive_clicked;
		colors[ImGuiCol_CheckMark] = color_text;
		colors[ImGuiCol_SliderGrab] = color_interactive;
		colors[ImGuiCol_SliderGrabActive] = color_interactive_clicked;
		colors[ImGuiCol_Button] = color_interactive;
		colors[ImGuiCol_ButtonHovered] = color_interactive_hovered;
		colors[ImGuiCol_ButtonActive] = color_interactive_clicked;
		colors[ImGuiCol_Header] = color_interactive;            // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
		colors[ImGuiCol_HeaderHovered] = color_interactive_hovered;
		colors[ImGuiCol_HeaderActive] = color_interactive_clicked;
		colors[ImGuiCol_Separator] = color_interactive;
		colors[ImGuiCol_SeparatorHovered] = color_interactive_hovered;
		colors[ImGuiCol_SeparatorActive] = color_interactive_clicked;
		colors[ImGuiCol_ResizeGrip] = color_interactive;
		colors[ImGuiCol_ResizeGripHovered] = color_interactive_hovered;
		colors[ImGuiCol_ResizeGripActive] = color_interactive_clicked;
		colors[ImGuiCol_Tab] = color_interactive;
		colors[ImGuiCol_TabHovered] = color_interactive_hovered;
		colors[ImGuiCol_TabActive] = color_interactive_clicked;
		colors[ImGuiCol_TabUnfocused] = color_interactive;
		colors[ImGuiCol_TabUnfocusedActive] = color_interactive;            // Might be called active, but it's active only because it's it's the only tab available, the user didn't really activate it
		colors[ImGuiCol_DockingPreview] = color_interactive_clicked;    // Preview overlay color when about to docking something
		colors[ImGuiCol_DockingEmptyBg] = color_interactive;            // Background color for empty node (e.g. CentralNode with no window docked into it)
		colors[ImGuiCol_PlotLines] = color_interactive;
		colors[ImGuiCol_PlotLinesHovered] = color_interactive_hovered;
		colors[ImGuiCol_PlotHistogram] = color_interactive;
		colors[ImGuiCol_PlotHistogramHovered] = color_interactive_hovered;
		colors[ImGuiCol_TextSelectedBg] = color_background;
		colors[ImGuiCol_DragDropTarget] = color_interactive_hovered;    // Color when hovering over target
		colors[ImGuiCol_NavHighlight] = color_background;             // Gamepad/keyboard: current highlighted item
		colors[ImGuiCol_NavWindowingHighlight] = color_background;             // Highlight window when using CTRL+TAB
		colors[ImGuiCol_NavWindowingDimBg] = color_background;             // Darken/colorize entire screen behind the CTRL+TAB window list, when active
		colors[ImGuiCol_ModalWindowDimBg] = color_background;             // Darken/colorize entire screen behind a modal window, when one is active

		// Spatial settings
		const auto font_size = 24.0f;
		const auto font_scale = 0.7f;
		const auto roundness = 2.0f;

		// Spatial
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowBorderSize = 1.0f;
		style.FrameBorderSize = 0.0f;
		style.ScrollbarSize = 20.0f;
		style.FramePadding = ImVec2(5, 5);
		style.ItemSpacing = ImVec2(6, 5);
		style.WindowMenuButtonPosition = ImGuiDir_Right;
		style.WindowRounding = roundness;
		style.FrameRounding = roundness;
		style.PopupRounding = roundness;
		style.GrabRounding = roundness;
		style.ScrollbarRounding = roundness;
		style.Alpha = 1.0f;
		*/
		const auto font_size = 24.0f;
		const auto font_scale = 0.7f;

		// Font
		auto& io = ImGui::GetIO();
		//const string dir_fonts = m_context->GetSubsystem<ResourceCache>()->GetResourceDirectory(ResourceDirectory::Fonts) + "/";
		io.Fonts->AddFontFromFileTTF("../Assets/Fonts/NanumGothic.ttf", font_size);
		io.FontGlobalScale = font_scale;
	}
}