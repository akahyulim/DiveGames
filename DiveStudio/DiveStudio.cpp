#include "DiveStudio.h"
#include "Renderer.h"
#include "GraphicsDevice.h"
#include "ImGUI/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

namespace Studio
{
	Studio::Studio()
	{
	}
	
	Studio::~Studio()
	{
	}

	void Studio::Initialize()
	{
		// 현재 Renderer 생성과정에서 오류가 발생한다.
		Dive::Runtime::Initialize();

		// ImGUI
		{
			auto pGraphicsDevice = Dive::Renderer::GetInstance().GetGraphicsDevice();

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			ImGui::StyleColorsDark();

			ImGui_ImplWin32_Init(m_hWnd);
			ImGui_ImplDX11_Init(pGraphicsDevice->GetDevice(), pGraphicsDevice->GetImmediateContext());
		}
	}

	void Studio::Render()
	{
		auto graphicsDevice = Dive::Renderer::GetInstance().GetGraphicsDevice();
		graphicsDevice->PresentBegin();

		// lua

		if (m_activePath)
		{
			m_activePath->Render();
		}

		Compose();

		// ImGUI
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// draw widget
			//for (auto& widget : m_widgets)
			{
			//	widget->Begin();
			//	widget->Tick();
			//	widget->End();
			}

			ImGui::Render();

			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}

		graphicsDevice->PresentEnd();
	}
}