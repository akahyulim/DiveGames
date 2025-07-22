#include "Editor.h"
#include "MenuBar.h"
#include "Views/WorldView.h"
#include "Views/GameView.h"
#include "Views/HierarchyView.h"
#include "Views/InspectorView.h"
#include "Views/LogView.h"

#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

constexpr float DEFAULT_FONT_SIZE = 16.0f;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK EditorMessageHandler(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

	switch (msg)
	{
	case WM_SIZE:
	{
		auto data = std::make_unique<std::pair<uint32_t, uint32_t>>(LOWORD(lParam), HIWORD(lParam));
		DV_FIRE_EVENT_DATA(Dive::eEventType::WindowResized, (void*)data.get());
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

	// main으로부터 argc를 받아 추린 후 엔진에 전달한다.
	Editor::Editor()
	{
		// 이것도 argc에 포함시켜 엔진 초기화과정에 편입시키는 게 맞다.
		LogManager::SetFilename("dive_editor.log");
		if (!Engine::Initialize())
		{
			DV_LOG(Editor, critical, "Engine 초기화 실패");
			throw std::runtime_error("Engine 초기화 실패");
		}
		Window::SetMessageCallback((LONG_PTR)EditorMessageHandler);
		//Window::Maximize();

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

		loadResources();

		// create widgets
		m_Widgets.emplace_back(std::make_unique<WorldView>(this));
		m_Widgets.emplace_back(std::make_unique<GameView>(this));
		m_Widgets.emplace_back(std::make_unique<HierarchyView>(this));
		m_Widgets.emplace_back(std::make_unique<InspectorView>(this));
		//m_Widgets.emplace_back(std::make_unique<LogView>(this));
		m_MenuBar = std::make_unique<MenuBar>(this);

		SetTitle();
	}

	Editor::~Editor()
	{
		for (auto& widget : m_Widgets)
			widget.reset();
		m_MenuBar.reset();

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		Engine::Shutdown();
	}

	void Editor::Run()
	{
		while (Window::Tick())
		{
			Engine::Tick();

			{
				beginUI();

				m_MenuBar->Draw();
				for (auto& widget : m_Widgets)
					widget->Draw();

				endUI();
			}

			Graphics::Present();
		}
	}

	ImFont* Editor::GetFont(eFontTypes type)
	{
		return m_Fonts[static_cast<size_t>(type)];
	}

	void Editor::SetTitle(const std::wstring& text)
	{
		std::wstring title = text.empty() ? L"Dive Editor" : L"Dive Editor - " + text;
		Window::SetTitle(title);
	}

	void Editor::loadResources()
	{
		// FONTS
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.FontDefault = m_Fonts[static_cast<size_t>(eFontTypes::Normal)] =
			io.Fonts->AddFontFromFileTTF("Assets/Fonts/NanumBarunGothic.ttf", DEFAULT_FONT_SIZE, nullptr, io.Fonts->GetGlyphRangesKorean());
		m_Fonts[static_cast<size_t>(eFontTypes::Bold)] =
			io.Fonts->AddFontFromFileTTF("Assets/Fonts/NanumBarunGothicBold.ttf", DEFAULT_FONT_SIZE, nullptr, io.Fonts->GetGlyphRangesKorean());
		m_Fonts[static_cast<size_t>(eFontTypes::Large)] =
			io.Fonts->AddFontFromFileTTF("Assets/Fonts/NanumBarunGothic.ttf", DEFAULT_FONT_SIZE * 1.5f, nullptr, io.Fonts->GetGlyphRangesKorean());
		m_Fonts[static_cast<size_t>(eFontTypes::Large_Bold)] =
			io.Fonts->AddFontFromFileTTF("Assets/Fonts/NanumBarunGothicBold.ttf", DEFAULT_FONT_SIZE * 1.5f, nullptr, io.Fonts->GetGlyphRangesKorean());
	}

	void Editor::beginUI()
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

	void Editor::endUI()
	{
		ImGui::End();

		auto deviceContext = Graphics::GetDeviceContext();

		auto renderTargetView = Graphics::GetBackBufferRTV();
		auto depthStencilView = Graphics::GetBackBufferDSV();
		deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

		float clearColors[4] = { 0.1f, 0.1f, 0.1f, 0.0f };
		deviceContext->ClearRenderTargetView(Graphics::GetBackBufferRTV(), clearColors);
		deviceContext->ClearDepthStencilView(Graphics::GetBackBufferDSV(), D3D11_CLEAR_DEPTH, 1.0f, 0);

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
