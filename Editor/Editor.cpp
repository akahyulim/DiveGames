#include "Editor.h"
#include "WidgetMenu.h"
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_internal.h"
#include "External/ImGui/imgui_impl_win32.h"
#include "External/ImGui/imgui_impl_dx11.h"

namespace Editor
{
	Editor::Editor()
	{
	}

	Editor::~Editor()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	
	bool Editor::Initialize()
	{
		// 여기에서 ini 파일로 윈도우를 재설정할 수 있다.
		// 그게 더 깔끔해 보인다.
		// 윈도우 크기 설정이 Backbuffer에 영향을 미치므로 시스템 생성 전에 하는거다.
		{
			// 이 파일경로는 멤버 변수로 저장하는 편이 낫다.
			auto filePath = Dive::FileSystemHelper::GetWorkingDirectory() + "editor.ini";
			Dive::IniHelper ini(filePath);
			bool bMaximize	= ini["Window"]["bMaximize"] << false;
			int width		= ini["Window"]["Width"] << 800;
			int height		= ini["Window"]["Height"] << 600;
			int posX		= (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
			int posY		= (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

			// 프로젝트별로 변경할거라면 이 역시 멤버 변수로 저장하는 편이 낫다?
			// 아니다. 디폴트 문자열이 있고(ini에 저장할 필요도 없다.)
			// Scene 로드 여부에 따라 scene 이름을 따를지 디폴트 이름을 쓸지 결정하는 편이 낫다.
			SetWindowText(m_hWnd, L"Test Text");

			if (!Dive::FileSystemHelper::FileExists(filePath))
			{
				ini["Window"]["Width"]		= width;
				ini["Window"]["Height"]		= height;
				ini["Window"]["bMaximize"]	= bMaximize;
			}

			RECT rcClient = { 0, 0, width, height };

			AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);

			SetWindowPos(m_hWnd, nullptr, posX, posY, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top), 0);

			ShowWindow(m_hWnd, bMaximize ? SW_MAXIMIZE : SW_SHOW);
			UpdateWindow(m_hWnd);
		}

		if (!Dive::Runtime::Initialize())
			return false;

		// ImGui 초기화
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			ImGui::StyleColorsDark();

			ImGui_ImplWin32_Init(m_hWnd);

			auto pDevice = Dive::Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
			auto pImmediateContext = Dive::Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
			ImGui_ImplDX11_Init(pDevice, pImmediateContext);
		}

		// Widget 생성
		{
			m_widgets.emplace_back(std::make_shared<WidgetMenu>());
		}

		// 모든 초기화를 끝마친 후 윈도우 show?

		APP_TRACE("Editor의 초기화에 성공하였습니다.");

		return true;
	}

	void Editor::Render()
	{
		auto graphicsDevice = Dive::Renderer::GetInstance().GetGraphicsDevice();
		graphicsDevice->PresentBegin();

		// ImGui에서 그리는 부분
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// 이 곳에서 Widget을 그릴 것이다.
			for (auto pWidget : m_widgets)
			{
				pWidget->Begin();
				pWidget->Tick();
				pWidget->End();
			}

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}

		// 이건 엔진에서 그리는 부분
		// 에디터에서는 RenderPath3D를 수정해야 한다.
		// Debug용 Render가 추가되고,
		// Scene이 다른 RenderTarget에 그려져야 하기 때문이다.
		if (m_activePath)
		{
			m_activePath->Render();
		}

		Compose();
		graphicsDevice->PresentEnd();
	}

	void Editor::ResizeWindow(unsigned int width, unsigned int height)
	{
		auto filePath = Dive::FileSystemHelper::GetWorkingDirectory() + "editor.ini";
		Dive::IniHelper ini(filePath);
		ini["Window"]["Width"]		= width;
		ini["Window"]["Height"]		= height;
		ini["Window"]["bMaximize"]	= IsZoomed(m_hWnd) ? true : false;
	}
}