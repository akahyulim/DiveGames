#include "Core/Window.h"
#include <memory>

int main()
{
	Dive::WindowInit windowData{};
	windowData.width = 1024;
	windowData.height = 768;
	windowData.title = L"SOKOBAN";

	auto window = std::make_unique<Dive::Window>(windowData);
	while (window->Run())
	{

	}

	return 0;
}