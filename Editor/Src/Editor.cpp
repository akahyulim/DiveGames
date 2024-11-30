#include "Editor.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_win32.h"
#include "imgui-docking/imgui_impl_dx11.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK EditorMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

namespace Dive
{
	Editor::Editor()
	{
	}

	int Editor::Run()
	{
		// 엔진 설정 및 초기화
		{
			Dive::LogManager::SetFilename("dive_editor.log");

			m_pEngine = std::make_unique<Dive::Engine>();
			if (!m_pEngine->Initialize(::GetModuleHandle(nullptr), 1280, 720, L"Dive_Editor"))
				return 1;

			if (!::SetWindowLongPtr(m_pEngine->GetWindowHandle(), GWLP_WNDPROC, (LONG_PTR)EditorMessageHandler))
			{
				DV_LOG(Window, err, "윈도우 프로시져 함수 변경 실패");
				return 1;
			}
		}

		// 에디터 설정 및 초기화
		{
			initializeImGui();
			DV_SUBSCRIBE_EVENT(Dive::eEventType::PostRender, DV_EVENT_HANDLER(OnPostRender));
		}

		// 실행
		m_pEngine->Run();

		// 에디터 셧다운
		{
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}

		// 엔진 셧다운
		m_pEngine->Shutdown();

		return 0;
	}

	void Editor::OnPostRender()
	{
		// 추후엔 백버퍼뷰를 직접 바인딩해야 한다.
		renderImGui();
	}

	void Editor::initializeImGui()
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
		ImGui_ImplWin32_Init(m_pEngine->GetWindowHandle());
		ImGui_ImplDX11_Init(m_pEngine->GetDevice(), m_pEngine->GetDeviceContext());

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

		DV_LOG(Editor, trace, "ImGui 초기화를 수행하였습니다.");
	}

	void Editor::renderImGui()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// frame 설정 - 위젯
		{
			ImGui::BeginMainMenuBar();

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("New"))
				{
					if (ImGui::MenuItem("Project"))
					{

					}
					if (ImGui::MenuItem("Scene"))
					{
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Open"))
				{
					if (ImGui::MenuItem("Project"))
					{

					}
					if (ImGui::MenuItem("Scene"))
					{
					}
					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Close"))
				{

				}
				if (ImGui::MenuItem("Close Project"))
				{

				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save"))
				{
				}
				if (ImGui::MenuItem("Save As..."))
				{

				}
				if (ImGui::MenuItem("Save Project"))
				{

				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					::PostQuitMessage(0);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("GameObject"))
			{
				if (ImGui::MenuItem("Create Empty"))
				{
				}

				if (ImGui::BeginMenu("3D Object"))
				{
					if (ImGui::MenuItem("Cube"))
					{
					}

					if (ImGui::MenuItem("Sphere"))
					{
					}

					if (ImGui::MenuItem("Cylinder"))
					{
					}

					if (ImGui::MenuItem("Cone"))
					{
					}

					if (ImGui::MenuItem("Plane"))
					{
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Light"))
				{
					if (ImGui::MenuItem("Directional"))
					{
					}

					if (ImGui::MenuItem("Point"))
					{
					}

					if (ImGui::MenuItem("Spot"))
					{
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Camera"))
				{
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}
		}

		ImGui::EndMainMenuBar();

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