#include "Editor.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_win32.h"
#include "imgui-docking/imgui_impl_dx11.h"

static Dive::Editor* s_pEditor = nullptr;

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

namespace Dive
{
	Editor::Editor(HINSTANCE hInstance, const std::string& title)
	{
		// load ini
		WindowProps props;
		props.Title = title;
		props.Width = 800;
		props.Height = 600;
		props.bMaximize = false;

		// create window
		createWindow(hInstance, props);

		// create d3d11 device
		createDeviceD3D();

		// intialize ImGui
		intializeImGui();

		// create engine

		DV_ASSERT(s_pEditor == nullptr);
		s_pEditor = this;

		ShowWindow(m_hWnd, SW_SHOW);
		SetForegroundWindow(m_hWnd);
		SetFocus(m_hWnd);
	}

	Editor::~Editor()
	{
		// destroy engine

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

	void Editor::Run()
	{
		bool done = false;
		while (!done)
		{
			MSG msg;
			ZeroMemory(&msg, sizeof(MSG));

			while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessageW(&msg);
				
				if (msg.message == WM_QUIT)
					done = true;
			}

			if (done)
				break;

			// 이 아래쪽에서 Update
			{
				// Start the Dear ImGui frame
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				drawPanels();

				// Render
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

				m_pSwapChain->Present(1, 0); // Present with vsync
			}
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

	void Editor::createWindow(HINSTANCE hInstance, const WindowProps& props)
	{
		if (hInstance == 0)
			hInstance = GetModuleHandle(NULL);

		std::wstring windowName;
		windowName.assign(props.Title.begin(), props.Title.end());

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

		int posX = (GetSystemMetrics(SM_CXSCREEN) - (int)props.Width) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - (int)props.Height) / 2;

		auto hWnd = CreateWindowW(windowName.c_str(), windowName.c_str(), WS_OVERLAPPEDWINDOW,
			posX, posY, props.Width, props.Height, nullptr, nullptr, hInstance, nullptr);

		DV_ASSERT(hWnd != NULL);

		m_hInstance = hInstance;
		m_hWnd = hWnd;
		m_Title = props.Title;
		m_Width = props.Width;
		m_Height = props.Height;
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
	}

	void Editor::drawPanels()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("New"))
				{
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Open"))
				{
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("GameObject"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Component"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}