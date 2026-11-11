#include "Game.h"

int main(int argc, char* argv[])
{
	Sokoban::Game game;
	return game.Run();
}

/*
int main()
{
	Dive::Window window(800, 600, L"SOKOBAN");
	Dive::Graphics graphics(&window);
	Dive::TextureManager::Get().Initialize(&graphics);	// 싱글톤인데 초기화 메서드는 별도로 존재하는 게 꺼림직하다. => 생각했으나 제미나이는 오히려 추천!
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
}*/