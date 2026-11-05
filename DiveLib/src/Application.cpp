#include "Core/Application.h"
#include "Core/Window.h"
#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

namespace Dive
{
	Application::Application() = default;
	Application::~Application() = default;

	void Application::Run()
	{
		Dive::WindowInit windowData{};
		windowData.width = 800;
		windowData.height = 600;
		windowData.title = L"SOKOBAN";
		m_window = std::make_unique<Window>(windowData);
		m_graphics = std::make_unique<Graphics>(m_window.get());
		m_input = std::make_unique<Input>(m_window->GetWindowHandle());

		Initialize();

		while (m_window->Run())
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// OnUpdate
			{
				m_input->Update();
				Update(0.016f);
			}

			// OnRenderUI()

			ImGui::Render();

			// Render
			{
				m_graphics->ClearBackbuffer();
				m_graphics->SetBackbuffer();

				Render();

				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

				m_graphics->EndRenderPass();
				m_graphics->SwapBuffers();
			}
		}

		Shutdown();
	}
}