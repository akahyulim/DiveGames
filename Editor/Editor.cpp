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

		// ImGui �ʱ�ȭ
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

		// Widget ����
		{
			m_widgets.emplace_back(std::make_shared<WidgetMenu>());
		}

		APP_TRACE("Editor�� �ʱ�ȭ�� �����Ͽ����ϴ�.");
	}

	void Editor::Render()
	{
		auto graphicsDevice = Dive::Renderer::GetInstance().GetGraphicsDevice();
		graphicsDevice->PresentBegin();

		// ImGui���� �׸��� �κ�
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// �� ������ Widget�� �׸� ���̴�.
			for (auto pWidget : m_widgets)
			{
				pWidget->Begin();
				pWidget->Tick();
				pWidget->End();
			}

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}

		// �̰� �������� �׸��� �κ�
		// �����Ϳ����� RenderPath3D�� �����ؾ� �Ѵ�.
		// Debug�� Render�� �߰��ǰ�,
		// Scene�� �ٸ� RenderTarget�� �׷����� �ϱ� �����̴�.
		if (m_activePath)
		{
			m_activePath->Render();
		}

		Compose();
		graphicsDevice->PresentEnd();
	}
}