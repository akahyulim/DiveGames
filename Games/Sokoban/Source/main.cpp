#include <memory>
#include "Core/Window.h"
#include "Graphics/Graphics.h"
#include "ImGui/ImGuiManager.h"
#include "Input/Input.h"
#include "Utilities/Timer.h"
#include "Sokoban.h"

int main()
{
	Dive::Window window(800, 600, L"SOKOBAN");
	Dive::Graphics graphics(&window);
	Dive::ImGuiManager imGuiMng(&graphics);
	Dive::Input input(window.GetWindowHandle());
	Dive::Timer timer;

	Sokoban::Sokoban game(&graphics, &input, &imGuiMng);

	while (!window.ShouldClose())
	{
		window.PollEvents();

		timer.Tick();
		input.Update();

		game.Update(timer.GetDeltaTime());

		if (imGuiMng.IsVisible())
		{
			game.Render();
			game.RenderUI();

			graphics.SwapBuffers();
		}
		else
		{
			//m_engine->Run();
			//m_engine->Present();	// Present가 SwapBuffers()다.
			graphics.SwapBuffers();
		}
	}

	game.Shutdown();

	return 0;
	/*
		while (!window.ShouldClose())
		{
			window.PollEvents();

			// UI 프레임 시작
			imguiManager.BeginFrame();

			// 게임 로직 및 UI 렌더링
			ImGui::Begin("Sokoban Controls");
			// ... 각종 UI 조작 ...
			ImGui::End();

			// 렌더링
			graphics.Clear(0.1f, 0.1f, 0.1f, 1.0f);
			// ... 게임 오브젝트 드로우 ...
        
			imguiManager.EndFrame();
			graphics.SwapBuffers();
		}
	*/
}