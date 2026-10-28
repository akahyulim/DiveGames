#include <memory>
#include "Core/Window.h"
#include "Graphics/Graphics.h"

int main()
{
	Dive::WindowInit windowData{};
	windowData.width = 800;
	windowData.height = 600;
	windowData.title = L"SOKOBAN";

	auto window = std::make_unique<Dive::Window>(windowData);

	auto graphics = std::make_unique<Dive::Graphics>(window.get());

	while (window->Run())
	{
		graphics->ClearBackbuffer();
		graphics->SetBackbuffer();
		graphics->EndRenderPass();
		graphics->SwapBuffers();
	}

	return 0;
}