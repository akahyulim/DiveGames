#include "LightViewer.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_win32.h"
#include "imgui-docking/imgui_impl_dx11.h"
//#include "imguizmo/ImGuizmo.h"

#include <commdlg.h>

// 이게 가장 먼저 호출되어야 한다...?
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LightViewer::LightViewer()
{
}

LightViewer::~LightViewer()
{
}

bool LightViewer::Initialize()
{
	auto pAppWindow = new Dive::Window;
	pAppWindow->SetTitle(L"LightViewer");
	if (!pAppWindow->Create(1600, 900))
		return false;
	::ShowWindow(pAppWindow->GetHandle(), SW_SHOW);
	::SetFocus(pAppWindow->GetHandle());

	Dive::GEngine->SetAppWindow(pAppWindow);
	if (!Dive::GEngine->Initialize())
		return false;
	Dive::GEngine->RegisterLogCategory("LightViewer");

	initializeImGui();


	Dive::World* pTestWorld = nullptr;
	for (uint32_t i = 0; i != 11; i++)
	{
		pTestWorld = Dive::World::CreateWorld();
		DV_LOG_INFO("LightViewer", "World name: {:s}", pTestWorld->GetName());
	}

	DV_LOG_TRACE("LightViewer", "초기화에 성공하였습니다.");

	return true;
}

void LightViewer::Release()
{
	DV_LOG_TRACE("LightViewer", "릴리즈를 수행합니다.");
	
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	DV_RELEASE(Dive::GEngine);
}

void LightViewer::Run()
{
	bool exit = false;
	while (!exit)
	{
		MSG msg{};
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}

		if (msg.message == WM_QUIT)
			exit = true;

		// 역시 임시 구현
		// App에 따라 커스텀 할 수 있어야 하기에 이런 구조가 되었다.
		// 여기에서 imGui를 출력하는 것부터 해보자.

		// frame
		{
			// start
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// frame 설정 - 위젯
		}

		// 이건 다른 렌더타겟에 그려야 하니 순서는 상관없다.
		// 허나 출력 위젯이 업데이트 되기 전에 그려야할 것 같긴 하다.
		Dive::GEngine->Draw();
		
		// Rendering ImGui 
		{
			ImGui::Render();
			//const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			//g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
			//g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			// 이것들이 뭘 의미하는 지 모르겠다.
			// Update and Render additional Platform Windows
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}

			//g_pSwapChain->Present(1, 0); // Present with vsync
			//g_pSwapChain->Present(0, 0); // Present without vsync
		}

		// 현재 렌더타겟 클리어를 포함하고 있다.
		// 위의 ImGui_ImplDX11_RenderDrawData()가 사이에 끼여야 할 수 있다.
		Dive::GEngine->Present();
	}
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
	ImGui_ImplWin32_Init(Dive::GEngine->GetAppWindow()->GetHandle());
	ImGui_ImplDX11_Init(Dive::GEngine->GetGraphics()->GetDevice(), Dive::GEngine->GetGraphics()->GetDeviceContext());

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

	DV_LOG_TRACE("LightViewer", "ImGui 초기화를 수행하였습니다.");
}

/*
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
*/
