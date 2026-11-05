#include "Core/Application.h"
#include "Core/Window.h"
#include "Graphics/Graphics.h"
#include "Input/Input.h"

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
			m_input->Update();

			Update(0.016f);

			m_graphics->ClearBackbuffer();
			m_graphics->SetBackbuffer();

			Render();

			m_graphics->EndRenderPass();
			m_graphics->SwapBuffers();
		}

		Shutdown();
	}
}