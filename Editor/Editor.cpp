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
	
	void Editor::Initialize()
	{
		Dive::Runtime::Initialize();

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

		APP_TRACE("Editor의 초기화에 성공하였습니다.");
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
}