#include "Editor.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_win32.h"
#include "imgui-docking/imgui_impl_dx11.h"
#include "imguizmo/ImGuizmo.h"
#include "Panels/MenuBarPanel.h"
#include "Panels/ScenePanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/AssetPanel.h"

DEFINE_APPLICATION_MAIN(Editor::Editor)

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Editor
{
	Editor::Editor(Dive::Context* pContext)
		: Dive::Application(pContext),
		m_pMenuBar(nullptr),
		m_pScene(nullptr),
		m_pHierarchy(nullptr),
		m_pInspector(nullptr),
		m_pAsset(nullptr)
	{
		SUBSCRIBE_EVENT(Dive::eEventType::BeginRender, EVENT_HANDLER_PARAM(OnBeginRender));
		SUBSCRIBE_EVENT(Dive::eEventType::EndRender, EVENT_HANDLER_PARAM(OnEndRender));
		SUBSCRIBE_EVENT(Dive::eEventType::WindowEvent, EVENT_HANDLER_PARAM(OnWindowEvent));
	}

	Editor::~Editor()
	{
	}
	
	void Editor::Setup()
	{
		// Engine Mode 설정 가능
		m_EngineParameters.title = L"Editor";
		m_EngineParameters.width = 1280;
		m_EngineParameters.height = 760;
		m_EngineParameters.positionX = m_EngineParameters.GetCenterPositionX();
		m_EngineParameters.positionY = m_EngineParameters.GetCenterPositionY();
	}
	
	void Editor::Start()
	{
		DV_LOG_CLIENT_DEBUG("call Editor's Start()");
		// UI 설정
		
		auto* pGraphics = m_pContext->GetSubsystem<Dive::Graphics>();
		
		// Initialize ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplWin32_Init(pGraphics->GetWindowHandle());
		ImGui_ImplDX11_Init(
			pGraphics->GetDevice(),
			pGraphics->GetDeviceContext()
		);
		
		// Set ImGui Theme
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// load resources
		float fontSize = 15.0f;
		//auto io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("Assets/Fonts/Nanum/NanumBarunGothic.ttf", fontSize);
		io.Fonts->AddFontFromFileTTF("Assets/Fonts/Nanum/NanumBarunGothicBold.ttf", fontSize);

		// Create Panels
		m_pMenuBar = std::make_unique<MenuBarPanel>(this);
		m_pScene = std::make_unique<ScenePanel>(this);
		m_pHierarchy = std::make_unique<HierarchyPanel>(this);
		m_pInspector = std::make_unique<InspectorPanel>(this);
		m_pAsset = std::make_unique<AssetPanel>(this);
	}
	
	void Editor::Stop()
	{
		DV_LOG_CLIENT_DEBUG("call Editor's Stop()");

		// destroy ImGui
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void Editor::OnBeginRender(const Dive::Event& e)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		// draw panels
		drawPanels();
	}

	void Editor::OnEndRender(const Dive::Event& e)
	{
		auto* pGraphics = m_pContext->GetSubsystem<Dive::Graphics>();
		auto* pDeviceContext = pGraphics->GetDeviceContext();
		auto* pDefaultRenderTargetView = pGraphics->GetDefaultRenderTargetView();

		const float clear_color_with_alpha[4]{ 0.1f, 0.1f, 0.1f, 0.0f };

		ImGui::Render();

		pDeviceContext->OMSetRenderTargets(1, &pDefaultRenderTargetView, NULL);
		pDeviceContext->ClearRenderTargetView(pDefaultRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void Editor::OnWindowEvent(const Dive::Event& e)
	{
		auto& evnt = dynamic_cast<const Dive::WindowEvent&>(e);

		ImGui_ImplWin32_WndProcHandler(
			evnt.m_hWnd,
			evnt.m_Msg,
			evnt.m_wParam,
			evnt.m_lParam
		);
	}
	
	// 그냥 OnBeginRender()에 넣는 편이 나을 것 같다.
	void Editor::drawPanels()
	{
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
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		// dock space에서의 최소 크기...
		style.WindowMinSize.x = 200.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSizeX;

		// Render Panels
		m_pMenuBar->Tick();
		m_pScene->Tick();
		m_pHierarchy->Tick();
		m_pInspector->Tick();
		m_pAsset->Tick();

		ImGui::End();
	}
}
