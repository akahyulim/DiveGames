#include <memory>
#include "Core/Window.h"
#include "Graphics/Graphics.h"
#include "Input/input.h"
#include "Stage.h"
#include "GamePlay.h"

#include "Graphics/ShaderManager.h"	// test

int main()
{
	Dive::WindowInit windowData{};
	windowData.width = 800;
	windowData.height = 600;
	windowData.title = L"SOKOBAN";

	auto window = std::make_unique<Dive::Window>(windowData);
	auto graphics = std::make_unique<Dive::Graphics>(window.get());
	auto input = std::make_unique<Dive::Input>(window->GetWindowHandle());

	Sokoban::GamePlay gamePlay;

	// graphics 때문에 위치가 애매하다.
	Dive::ShaderProgramDesc desc;
	desc.name = "Test";
	desc.vsPath = "Assets/Shaders/Test.hlsl";
	desc.psPath = "Assets/Shaders/Test.hlsl";
	desc.ilType = Dive::eInputLayout::None;
	Dive::ShaderManager::Get().CreateShaderProgram(graphics.get(), desc);

	while (window->Run())
	{
		input->Update();

		// deltaTime과 input을 전달하라고 추천한다.
		gamePlay.Update();

		graphics->ClearBackbuffer();
		graphics->SetBackbuffer();

		gamePlay.Render(graphics.get());

		graphics->EndRenderPass();
		graphics->SwapBuffers();
	}

	return 0;
}