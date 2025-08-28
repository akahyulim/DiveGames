#include "Editor.h"
#include "MenuBar.h"
#include "views/WorldView.h"
#include "views/GameView.h"
#include "views/HierarchyView.h"
#include "views/InspectorView.h"

#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

constexpr LPCWCH DV_TITLE = L"Dive Editor";
constexpr float DEFAULT_FONT_SIZE = 20.0f;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK EditorMessageHandler(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

	switch (msg)
	{
	case WM_SIZE:
	{
		DV_FIRE_EVENT(Dive::eEventType::WindowResized);
		return 0;
	}
	case WM_CLOSE:
	{
		Dive::Window::Close();
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

namespace Dive
{
	World* EditorContext::ActiveWorld = nullptr;
	GameObject* EditorContext::Selected = nullptr;
	GameObject* EditorContext::EditorCamera = nullptr;
	GameObject* EditorContext::MainCamera = nullptr;

	Editor::Editor()
		: m_fonts{}
	{
		DV_SUBSCRIBE_EVENT(eEventType::PostRender, DV_EVENT_HANDLER(OnPostRender));
	}

	void Editor::Setup()
	{
		LogManager::SetFilename("dive_editor.log");
	}
	
	void Editor::Start()
	{
		SetTitle();
		Window::SetMessageCallback((LONG_PTR)EditorMessageHandler);
		Window::Maximize();
		
		// ImGui 초기화
		{
			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-WorldView / Platform Windows

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();

			ImGuiStyle& style = ImGui::GetStyle();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				style.WindowRounding = 0.0f;
				style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			}

			// Setup Platform/Renderer backends
			ImGui_ImplWin32_Init(Window::GetWindowHandle());
			ImGui_ImplDX11_Init(Graphics::GetDevice(), Graphics::GetDeviceContext());

			// 스타일 수정
			{
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
			}
		}

		// Load Resources
		{
			// fonts
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.FontDefault = m_fonts[static_cast<size_t>(eFontTypes::Normal)] =
				io.Fonts->AddFontFromFileTTF("Assets/Fonts/NanumBarunGothic.ttf", DEFAULT_FONT_SIZE, nullptr, io.Fonts->GetGlyphRangesKorean());
			m_fonts[static_cast<size_t>(eFontTypes::Bold)] =
				io.Fonts->AddFontFromFileTTF("Assets/Fonts/NanumBarunGothicBold.ttf", DEFAULT_FONT_SIZE, nullptr, io.Fonts->GetGlyphRangesKorean());
			m_fonts[static_cast<size_t>(eFontTypes::Large)] =
				io.Fonts->AddFontFromFileTTF("Assets/Fonts/NanumBarunGothic.ttf", DEFAULT_FONT_SIZE * 1.5f, nullptr, io.Fonts->GetGlyphRangesKorean());
			m_fonts[static_cast<size_t>(eFontTypes::Large_Bold)] =
				io.Fonts->AddFontFromFileTTF("Assets/Fonts/NanumBarunGothicBold.ttf", DEFAULT_FONT_SIZE * 1.5f, nullptr, io.Fonts->GetGlyphRangesKorean());

			// texture2D
			ResourceManager::GetOrLoad<Texture2D>("Assets/Textures/dmc.jpg");
			ResourceManager::GetOrLoad<Texture2D>("Assets/Textures/DokeV.jpeg");
			ResourceManager::GetOrLoad<Texture2D>("Assets/Textures/relaxed_morning.jpg");
			ResourceManager::GetOrLoad<Texture2D>("Assets/Textures/sky_daytime_blue.jpg");
			ResourceManager::GetOrLoad<Texture2D>("Assets/Textures/stone01.tga");
		}

		// create views
		m_views.emplace_back(std::make_unique<WorldView>(this));
		m_views.emplace_back(std::make_unique<GameView>(this));
		m_views.emplace_back(std::make_unique<HierarchyView>(this));
		m_views.emplace_back(std::make_unique<InspectorView>(this));
		//m_views.emplace_back(std::make_unique<LogView>(this));
		m_menuBar = std::make_unique<MenuBar>(this);

		DV_LOG(Editor, info, "초기화 성공");
	}

	void Editor::Stop()
	{
		for (auto& widget : m_views)
			widget.reset();
		m_menuBar.reset();

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		DV_LOG(Editor, info, "셧다운 성공");

		m_engine->Shutdown();
	}

	void Editor::OnPostRender()
	{
		// Begin UI Frame
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			static bool dockspaceOpen = true;
			static bool opt_fullscreen_persistant = true;
			bool opt_fullscreen = opt_fullscreen_persistant;
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

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

			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			{
				window_flags |= ImGuiWindowFlags_NoBackground;
			}

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
			ImGui::PopStyleVar();

			if (opt_fullscreen)
			{
				ImGui::PopStyleVar(2);
			}

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
		}

		// Compose UI
		{
			m_menuBar->ComposeUI();
			for (auto& view : m_views)
				view->ComposeUI();
		}

		// End UI Fmame
		{
			ImGui::End();

			auto dc = Graphics::GetDeviceContext();

			auto renderTargetView = Graphics::GetRenderTargetView();
			auto depthStencilView = Graphics::GetDepthStencilView();
			dc->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

			float clearColors[4] = { 0.1f, 0.1f, 0.1f, 0.0f };
			dc->ClearRenderTargetView(renderTargetView, clearColors);
			dc->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			ImGuiIO& io = ImGui::GetIO(); (void)io;
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}
	}

	void Editor::SetTitle(const std::wstring& worldName)
	{
		std::wstring title = DV_TITLE;
		if (!worldName.empty()) 
			title += L" - " + worldName;

		Window::SetTitle(title);
	}
}