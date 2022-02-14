#include "Editor.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_win32.h"
#include "imgui-docking/imgui_impl_dx11.h"
#include "Panels/MenuBarPanel.h"
#include "Panels/ScenePanel.h"

static Editor* s_pEditor = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
	{
		if (s_pEditor)
		{
			if (s_pEditor->GetD3dDevice() != nullptr && wParam != SIZE_MINIMIZED)
			{
				s_pEditor->CleanupRenderTarget();
				s_pEditor->GetSwapChain()->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
				s_pEditor->CreateRenderTarget();
			}

			// 여기에서 Engine에 변경된 크기를 전달할 수 있다.
		}
		return 0;
	}
	case WM_DESTROY:
	{
		::PostQuitMessage(0);
		return 0;
	}
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

Editor::Editor(HINSTANCE hInstance, const std::string& title)
{
	// load ini
	m_Title = title;
	m_Width = 1280;
	m_Height = 720;
	m_bVSync = false;
	m_bFullScreen = false;

	// create window
	createWindow(hInstance);

	// create d3d11 device
	createDeviceD3D();

	// intialize ImGui
	intializeImGui();

	// create engine
	Dive::WindowData data;
	data.hWnd = m_hWnd;
	// 처음부터 ScenePanel의 크기를 주는게 낫지 않을까?
	data.Width = m_Width;
	data.Height = m_Height;
	data.bVSync = m_bVSync;
	data.bFullScreen = m_bFullScreen;
	Dive::CreateEngine(&data);

	DV_ASSERT(s_pEditor == nullptr);
	s_pEditor = this;

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);
}

Editor::~Editor()
{
		
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

		// 이 아래쪽에서 Update
		{
			Dive::Update();
			Dive::Render();

			// Begin
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			drawPanels();

			// End
			const float clear_color_with_alpha[4] = { 0.1f, 0.1f, 0.1f, 0.0f };
			ImGuiIO& io = ImGui::GetIO();

			ImGui::Render();
			m_pD3dDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);
			m_pD3dDeviceContext->ClearRenderTargetView(m_pRenderTargetView, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			// Update and Render additional Platform Windows
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}

			m_pSwapChain->Present(m_bVSync ? 1 : 0, 0);
		}
	}
}

void Editor::Shutdown()
{
	// delete panels
	if (m_Panels.empty())
	{
		for (auto pPanel : m_Panels)
		{
			DV_DELETE(pPanel);
		}
		m_Panels.clear();
	}

	// destroy engine
	Dive::DestroyEngine();

	// destroy ImGui
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	// destroy d3d11 device
	{
		cleanupDeviceD3D();
	}

	// destroy window
	{
		std::wstring windowName;
		windowName.assign(m_Title.begin(), m_Title.end());
		::DestroyWindow(m_hWnd);
		::UnregisterClass(windowName.c_str(), m_hInstance);
	}
}

void Editor::CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	m_pD3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	pBackBuffer->Release();
}

void Editor::CleanupRenderTarget()
{
	DV_RELEASE(m_pRenderTargetView);
}

void Editor::createWindow(HINSTANCE hInstance)
{
	if (hInstance == 0)
		hInstance = GetModuleHandle(NULL);

	std::wstring windowName;
	windowName.assign(m_Title.begin(), m_Title.end());

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = windowName.c_str();
	wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	DV_ASSERT(RegisterClassEx(&wcex));

	int posX = (GetSystemMetrics(SM_CXSCREEN) - (int)m_Width) / 2;
	int posY = (GetSystemMetrics(SM_CYSCREEN) - (int)m_Height) / 2;

	auto hWnd = CreateWindowW(windowName.c_str(), windowName.c_str(), WS_OVERLAPPEDWINDOW,
		posX, posY, m_Width, m_Height, nullptr, nullptr, hInstance, nullptr);

	DV_ASSERT(hWnd != NULL);

	m_hInstance = hInstance;
	m_hWnd = hWnd;
}
	
void Editor::createDeviceD3D()
{
	DV_ASSERT(m_hWnd);

	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	auto hResult = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pD3dDevice, &featureLevel, &m_pD3dDeviceContext);
	DV_ASSERT(hResult == S_OK);

	CreateRenderTarget();
}

void Editor::cleanupDeviceD3D()
{
	CleanupRenderTarget();

	DV_RELEASE(m_pSwapChain);
	DV_RELEASE(m_pD3dDevice);
	DV_RELEASE(m_pD3dDeviceContext);
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
	ImGui_ImplDX11_Init(m_pD3dDevice, m_pD3dDeviceContext);

	// custom style & resource
	setDarkThemeColors();
	loadResources();

	// create panels
	m_Panels.emplace_back(new MenuBarPanel(this));
	m_Panels.emplace_back(new ScenePanel(this));
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
	// 폰트 여러개를 등록한 후 사용할 수 있는지 확인 필요 => 배열로 접근할 수 있는듯
	// 만약 개수가 적다면 setDarkThemeColors()의 이름을 적절히 변환시킨 후 포함

	float fontSize = 16.0f;
	auto io = ImGui::GetIO();
	io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Nanum/NanumBarunGothic.ttf", fontSize);
}

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
	style.WindowMinSize.x = 100.0f;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	style.WindowMinSize.x = minWinSizeX;
		
	// Render Panels
	if (!m_Panels.empty())
	{
		for (auto pPanel : m_Panels)
			pPanel->Tick();
	}
	
	ImGui::Begin("Hierarchy");
	ImGui::End();
	ImGui::Begin("Inspector");
	ImGui::End();
	ImGui::Begin("Asset");
	ImGui::End();

	ImGui::End();
}