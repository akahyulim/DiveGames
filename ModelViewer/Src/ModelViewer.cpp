#include "ModelViewer.h"
#include "SceneViewCamera.h"

#include <imgui-docking/imgui.h>
#include <imgui-docking/imgui_impl_win32.h>
#include <imgui-docking/imgui_impl_dx11.h>
#include <Imgui-docking/imgui_internal.h>
#include <imgui-docking/imgui_stdlib.h>

DEFINE_APPLICATION_MAIN(ModelViewer)

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ModelViewer::ModelViewer()
	: m_pCamera(nullptr)
	, m_pLoadedModel(nullptr)
	, m_pSelectedNode(nullptr)
{
}

void ModelViewer::Setup()
{
	// Engine Setup
	Dive::Graphics::SetWindowTitle(L"ModelViewer");

	// Subscribe Events 
	SUBSCRIBE_EVENT(Dive::eEventType::PostRender, EVENT_HANDLER_PARAM(OnPostRender));
	SUBSCRIBE_EVENT(Dive::eEventType::WindowEvent, EVENT_HANDLER_PARAM(OnWindowEvent));
}

void ModelViewer::Start()
{
	// initialize ImGui
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 
		io.ConfigWindowsResizeFromEdges = true;
		io.ConfigViewportsNoTaskBarIcon = true;
		io.ConfigViewportsNoDecoration = true; // aka borderless but with ImGui min, max and close buttons
		io.IniFilename = "ModelViewer.ini";

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplWin32_Init(Dive::Graphics::GetWindowHandle());
		ImGui_ImplDX11_Init(
			Dive::Graphics::GetDevice(),
			Dive::Graphics::GetDeviceContext()
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
		io.Fonts->AddFontFromFileTTF("Assets/Fonts/NanumBarunGothic.ttf", fontSize);
		io.Fonts->AddFontFromFileTTF("Assets/Fonts/NanumBarunGothicBold.ttf", fontSize);
	}

	// create default scene & layer
	{
		// camera
		m_pCamera = Dive::Scene::CreateGameObject("MainCamera");
		Dive::Camera* pCamera = m_pCamera->AddComponent<SceneViewCamera>();
		pCamera->SetViewSize(static_cast<float>(Dive::Graphics::GetWindowWidth()), static_cast<float>(Dive::Graphics::GetWindowHeight()));
		m_pCamera->GetComponent<Dive::Transform>()->SetPosition(0.0f, 10.0f, -10.0f);
		//m_pCamera->GetComponent<Dive::Transform>()->SetLookAt(0.0f, 0.0f, 0.0f);
		
		// skydome
		// 누가 관리하냐...?
		auto pSkydome = new Dive::Skydome();
		m_pCamera->GetComponent<Dive::Camera>()->SetSkydome(pSkydome);
		
		// bottom
		auto pDiffTex = Dive::Texture2D::GetGrayTexture();		// 아에 rgba를 전달하면 텍스쳐를 만들어 주는게 낫지 않나?
		auto pBottomMat = new Dive::Material();
		pBottomMat->SetDiffuseColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		pBottomMat->SetTexture(Dive::eTextureUnit::Diffuse, pDiffTex);
		auto pBottom = Dive::Scene::CreateGameObject("Bottom");
		auto pMeshRenderer = pBottom->AddComponent<Dive::MeshRenderer>();
		pMeshRenderer->SetMesh(Dive::CreatePlane(128.0f, 128.0f));
		pMeshRenderer->SetMaterial(pBottomMat);

		// lights
		// 그림판 3D는 디렉셔널 라이트와 포인트 라이트 둘을 사용하는 듯 하다.

		// layer - 추후 Scene의 static을 제외하고 레이어에 직접 추가해야 한다.
		Dive::Layer* pLayer = new Dive::Layer();
		Dive::Renderer::PushLayer(pLayer);

		// 좀 애매한가...?
		// 결국 카메라 관리 주체가 누구냐의 문제이다.
		// 구현이 가장 비슷한 스파르탄의 경우 씬은 다른 엔터티와 같이 구성하여 생성할 뿐이고
		// 사용자인 렌더러는 씬에서 가져온 엔터티들을 분류하는 과정에서 카메라를 관리하며 사용할 뿐이다.
		// 그런데 이러한 과정에 오직 하나의 카메라만 존재할 때 의미가 있다.
		// 반면 스파키, 헤이즐의 경우 카메라가 컴포넌트가 아닌 단일 클래스로 구현되어 있고
		// 생성 및 관리는 어플리케이션 클래스에서 상황에 맞게 대응한다.
		// 따라서 두 개 이상의 카메라 사용이 가능해진다.
		m_MenuTabsPanel.Initialize(m_pCamera);
	}
}

void ModelViewer::Stop()
{
	// destroy ImGui
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}

void ModelViewer::OnPostRender(const Dive::Event& e)
{
	// ImGui BeginFrame
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	// dock space main window
	{
		const auto window_flags =
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;

		// Set window position and size
		float offset_y = 0;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + offset_y));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - offset_y));
		ImGui::SetNextWindowViewport(viewport->ID);

		// Set window style
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowBgAlpha(0.0f);

		// Begin window
		std::string name = "##main_window";
		bool open = true;
		ImGui::Begin(name.c_str(), &open, window_flags);
		ImGui::PopStyleVar(3);

		// Begin dock space
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			// Dock space
			const auto window_id = ImGui::GetID(name.c_str());
			if (!ImGui::DockBuilderGetNode(window_id))
			{
				// Reset current docking state
				ImGui::DockBuilderRemoveNode(window_id);
				ImGui::DockBuilderAddNode(window_id, ImGuiDockNodeFlags_None);
				ImGui::DockBuilderSetNodeSize(window_id, ImGui::GetMainViewport()->Size);

				ImGuiID dock_main_id = window_id;
				ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);

				// Dock windows
				ImGui::DockBuilderDockWindow("MenuTabs", dock_right_id);
				ImGui::DockBuilderDockWindow("SceneView", dock_main_id);

				ImGui::DockBuilderFinish(dock_main_id);
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
			ImGui::DockSpace(window_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
			ImGui::PopStyleVar();
		}
	}

	// panels
	{
		// info - 추후 텍스트로 화면에 직접 출력
		{
			ImGuiIO& io = ImGui::GetIO();

			ImGui::Begin("Info");

			POINT pt;
			pt.x = io.MousePos.x;
			pt.y = io.MousePos.y;
			ScreenToClient(Dive::Graphics::GetWindowHandle(), &pt);

			ImGui::Text("fps: %f", io.Framerate);
			ImGui::Text("sceneView size: %f x %f", Dive::Renderer::GetResolutionRender().x, Dive::Renderer::GetResolutionRender().y);
			ImGui::Text("window size: %f x %f", io.DisplaySize.x, io.DisplaySize.y);
			ImGui::Text("io mouse pos: %f, %f", (float)pt.x, (float)pt.y);//io.MousePos.x, io.MousePos.y);
			ImGui::Text("input mouse pos: %f, %f", Dive::Input::GetMousePosition().x, Dive::Input::GetMousePosition().y);

			ImGui::End();

		}

		m_SceneViewPanel.OnRender();
		m_MenuTabsPanel.OnRender();
	}

	ImGui::End();	// main window

	// ImGui EndFrame
	{
		// clear backbuffer
		{
			auto* pDeviceContext = Dive::Graphics::GetDeviceContext();
			auto* pDefaultRenderTargetView = Dive::Graphics::GetDefaultRenderTargetView();

			auto* pRTV = Dive::Graphics::GetDefaultRenderTargetView();
			auto* pDSV = Dive::Graphics::GetDefaultDepthStencilView();

			Dive::Graphics::SetRenderTargetView(0, pRTV);
			Dive::Graphics::SetDepthStencilView(pDSV);

			Dive::Graphics::ClearViews(Dive::eClearTarget::Color | Dive::eClearTarget::Depth, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0);
			pDeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);
		}

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(static_cast<float>(Dive::Graphics::GetWindowWidth()), static_cast<float>(Dive::Graphics::GetWindowHeight()));

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
}

void ModelViewer::OnWindowEvent(const Dive::Event& e)
{
	auto& evnt = dynamic_cast<const Dive::WindowEvent&>(e);

	ImGui_ImplWin32_WndProcHandler(
		evnt.m_hWnd,
		evnt.m_Msg,
		evnt.m_wParam,
		evnt.m_lParam);
}