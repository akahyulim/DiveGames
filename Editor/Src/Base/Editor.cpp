#include "Editor.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_win32.h"
#include "imgui-docking/imgui_impl_dx11.h"
#include "imguizmo/ImGuizmo.h"
#include "Panels/MenuBarPanel.h"
#include "Panels/ScenePanel.h"
#include "panels/HierarchyPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/AssetPanel.h"
#include "Importer/ModelImporter.h"

static Editor* s_pEditor = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (s_pEditor)
	{
		Dive::WindowData data;
		data.hWnd	= hWnd;
		data.msg	= msg;
		data.wParam = wParam;
		data.lParam = lParam;
		data.Width	= s_pEditor->GetWidth();
		data.Height = s_pEditor->GetHeight();

		if (msg == WM_DISPLAYCHANGE || msg == WM_SIZE)
		{
			data.Width	= static_cast<unsigned int>(lParam & 0xffff);
			data.Height = static_cast<unsigned int>((lParam >> 16) & 0xffff);
		}

		if (msg == WM_SYSCOMMAND)
		{
			data.bMinimize = wParam == SC_MINIMIZE;
			data.bMaximize = wParam == SC_MAXIMIZE;
		}

		if (msg == WM_CLOSE)
		{
			::PostQuitMessage(0);
			return 0;
		}

		s_pEditor->OnWindowMessage(data);
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

Editor::Editor(HINSTANCE hInstance, const std::string& title)
{
	// temp: 추후 load ini
	m_Title			= title;
	m_Width			= 1280;
	m_Height		= 720;
	m_bVSync		= true;
	m_bFullScreen	= false;

	// create window
	createWindow(hInstance);

	// create engine
	Dive::WindowData data;
	data.hWnd			= m_hWnd;
	data.Width			= GetWidth();
	data.Height			= GetHeight();
	data.bVSync			= m_bVSync;
	data.bFullScreen	= m_bFullScreen;
	auto pEngine		= Dive::CreateEngine(&data);

	// 위치가 애매하다.
	pEngine->SetActiveRenderPath(static_cast<Dive::RenderPath*>(&m_EditorRenderPath));
	m_EditorRenderPath.SetSceneViewCamera(&m_SceneViewCamera);

	// intialize ImGui
	intializeImGui();

	DV_ASSERT(s_pEditor == nullptr);
	s_pEditor = this;

	// Editor Camera 복사 생성 필요??

	m_pModelImporter = new ModelImporter;

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);
}

Editor::~Editor()
{
	//Shutdown();
}

void Editor::OnWindowMessage(const Dive::WindowData& data)
{
	auto pDiveEngine = Dive::GetCurrentEngine();
	if (!pDiveEngine)
		return;
		
	ImGui_ImplWin32_WndProcHandler(data.hWnd, data.msg, data.wParam, data.lParam);
	
	pDiveEngine->SetWindowData(data);
}

void Editor::Run()
{
	while (!m_bDone)
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
				
			if (msg.message == WM_QUIT)
			{
				Close();
			}
		}

		if (m_bDone)
			break;

		{
			// Scene이 변경될 수 있어 명시적 Set이 필요하다
			// 다만 이 부분은 이벤트로도 처리가 가능하다.
			m_EditorRenderPath.SetActiveScene(m_pMenuBar->GetActiveScene());
			
			// 현재 둘이 나란히 붙어 있지만 Wicked의 경우 Update가 분화되어 있고
			// 호출 단계도 나누어진다.
			Dive::GetCurrentEngine()->Update();
			// eSceneMode에 따라 구분될 수 있어야 한다.
			Dive::GetCurrentEngine()->Render();

			// Begin
			{
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();
				ImGuizmo::BeginFrame();
			}

			drawPanels();

			// End: 내부에서 다시 Begin / End가 들어가있다.
			{
				auto& graphicsDevice = Dive::Renderer::GetGraphicsDevice();
				auto pImmediateContext = graphicsDevice.GetImmediateContext();
				auto pSwapChain = graphicsDevice.GetSwapChain();
				auto pMainRenderTargetView = graphicsDevice.GetMainRenderTargetView();

				const float clear_color_with_alpha[4] = { 0.1f, 0.1f, 0.1f, 0.0f };
				ImGuiIO& io = ImGui::GetIO();

				ImGui::Render();

				pImmediateContext->OMSetRenderTargets(1, &pMainRenderTargetView, NULL);
				pImmediateContext->ClearRenderTargetView(pMainRenderTargetView, clear_color_with_alpha);
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

				// Update and Render additional Platform Windows
				if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					ImGui::UpdatePlatformWindows();
					ImGui::RenderPlatformWindowsDefault();
				}

				pSwapChain->Present(m_bVSync ? 1 : 0, 0);
			}
		}
	}
}

void Editor::Shutdown()
{
	// delete importer
	DV_DELETE(m_pModelImporter);

	// delete panels
	DV_DELETE(m_pMenuBar);
	DV_DELETE(m_pScene);
	DV_DELETE(m_pHierarchy);
	DV_DELETE(m_pInspector);
	DV_DELETE(m_pAsset);

	// destroy ImGui
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	// destroy engine
	Dive::DestroyEngine();

	// destroy window
	{
		std::wstring windowName;
		windowName.assign(m_Title.begin(), m_Title.end());
		::DestroyWindow(m_hWnd);
		::UnregisterClass(windowName.c_str(), m_hInstance);
	}
}

unsigned int Editor::GetWidth() const
{
	if(m_hWnd == 0)
		return m_Width;

	RECT rt;
	::GetClientRect(m_hWnd, &rt);

	return static_cast<unsigned int>(rt.right - rt.left);
}

unsigned int Editor::GetHeight() const
{
	if (m_hWnd == 0)
		return m_Height;

	RECT rt;
	::GetClientRect(m_hWnd, &rt);

	return static_cast<unsigned int>(rt.bottom - rt.top);
}

void Editor::createWindow(HINSTANCE hInstance)
{
	if (hInstance == 0)
		hInstance = GetModuleHandle(NULL);

	std::wstring windowName;
	windowName.assign(m_Title.begin(), m_Title.end());

	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(nullptr, IDI_APPLICATION);
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= nullptr;
	wcex.lpszClassName	= windowName.c_str();
	wcex.hIconSm		= LoadIcon(nullptr, IDI_APPLICATION);
	DV_ASSERT(RegisterClassEx(&wcex));

	int posX = (GetSystemMetrics(SM_CXSCREEN) - (int)m_Width) / 2;
	int posY = (GetSystemMetrics(SM_CYSCREEN) - (int)m_Height) / 2;

	auto hWnd = CreateWindowW(windowName.c_str(), windowName.c_str(), WS_OVERLAPPEDWINDOW,
		posX, posY, m_Width, m_Height, nullptr, nullptr, hInstance, nullptr);

	DV_ASSERT(hWnd != NULL);

	m_hInstance = hInstance;
	m_hWnd		= hWnd;
}

void Editor::intializeImGui()
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
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(
		Dive::Renderer::GetGraphicsDevice().GetDevice(), 
		Dive::Renderer::GetGraphicsDevice().GetImmediateContext());

	// custom style & resource
	setDarkThemeColors();
	loadResources();

	// create panels
	m_pMenuBar		= new MenuBarPanel(this);
	m_pScene		= new ScenePanel(this);
	m_pHierarchy	= new HierarchyPanel(this);
	m_pInspector	= new InspectorPanel(this);
	m_pAsset		= new AssetPanel(this);
}

void Editor::setDarkThemeColors()
{
	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg]			= ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

	// Headers
	colors[ImGuiCol_Header]				= ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_HeaderHovered]		= ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_HeaderActive]		= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Buttons
	colors[ImGuiCol_Button]				= ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_ButtonHovered]		= ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_ButtonActive]		= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Frame BG
	colors[ImGuiCol_FrameBg]			= ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_FrameBgHovered]		= ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_FrameBgActive]		= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Tabs
	colors[ImGuiCol_Tab]				= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabHovered]			= ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
	colors[ImGuiCol_TabActive]			= ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
	colors[ImGuiCol_TabUnfocused]		= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

	// Title
	colors[ImGuiCol_TitleBg]			= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgActive]		= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed]	= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
}

void Editor::loadResources()
{
	// 아이콘 텍스쳐
	// Panel의 아이콘 텍스처는 각각의 객체에서 로드

	// 폰트를 배열로 관리할 수 있는 것 같은데
	// 예상했던 것처럼 돌아가지 않는다...
	float fontSize = 15.0f;
	auto io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("Assets/Fonts/Nanum/NanumBarunGothic.ttf", fontSize);
	io.Fonts->AddFontFromFileTTF("Assets/Fonts/Nanum/NanumBarunGothicBold.ttf", fontSize);
	//io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Nanum/NanumBarunGothic.ttf", fontSize);
	// 여기에서 대입이 안되는 것 같다. 
}

// 현재 Docking + Panel Object + Panel hard cording이 합쳐져 좀 길다.
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