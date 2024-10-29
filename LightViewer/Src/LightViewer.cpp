#include "LightViewer.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_win32.h"
#include "imgui-docking/imgui_impl_dx11.h"
//#include "imguizmo/ImGuizmo.h"

#include <commdlg.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

DEFINE_APPLICATION_MAIN(LightViewer)

LightViewer::LightViewer()
	: m_pGraphics(nullptr)
	, m_pInput(nullptr)
	, m_pMainCam(nullptr)
	, m_CamDefaultPos(0.0f, 10.0f, -10.0f)
{
}

LightViewer::~LightViewer()
{
}

void LightViewer::OnSetup()
{
	m_Title = L"Light Viewer";
	m_WindowWidth = 1600;
	m_WindowHeight = 900;
}

void LightViewer::OnStart()
{
	m_pGraphics = Dive::Graphics::GetInstance();
	m_pInput = Dive::Input::GetInstance();

	m_pGraphics->SetWindowTitle(m_Title);

	initializeImGui();

	// create scene
	{
		Dive::Scene* pActiveScene = Dive::GetSceneManager()->CreateScene("Base");
		Dive::GetSceneManager()->SetActiveScene(pActiveScene);

		// main camera
		m_pMainCam = pActiveScene->CreateGameObject("MainCam");
		auto pCamCom = m_pMainCam->AddComponent<Dive::Camera>();
		pCamCom->SetBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
		m_pMainCam->SetPosition(m_CamDefaultPos);
		m_pMainCam->LookAt(0.0f, 0.0f, 0.0f);
	}

	// setup renderLayer
	{
		auto pViewScreen = new Dive::ViewScreen(m_pMainCam->GetComponent<Dive::Camera>(), Dive::eRenderPathType::Deferred);
		Dive::Renderer::GetInstance()->SetViewScreen(0, pViewScreen);
	}

	// subscribe events
	{
		SUBSCRIBE_EVENT(Dive::eEventType::PostUpdate, EVENT_HANDLER_PARAM(HandleUpdate));
	}
}

void LightViewer::OnStop()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void LightViewer::HandleUpdate(const Dive::Event& e)
{
//	ImGui_ImplWin32_WndProcHandler(data.hWnd, data.msg, data.wParam, data.lParam);
}

void LightViewer::initializeImGui()
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
	ImGui_ImplWin32_Init(Dive::Graphics::GetInstance()->GetWindowHandle());
	ImGui_ImplDX11_Init(Dive::Graphics::GetInstance()->GetDevice(), Dive::Graphics::GetInstance()->GetDeviceContext());

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

	// load resources
	float fontSize = 15.0f;
	io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/NanumBarunGothic.ttf", fontSize);
	io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/NanumBarunGothicBold.ttf", fontSize);
}
