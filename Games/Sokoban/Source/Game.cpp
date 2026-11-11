#include "Game.h"
#include "Core/Window.h"
#include "Graphics/Graphics.h"
#include "ImGui/ImGuiManager.h"
#include "Input/Input.h"
#include "Utilities/Timer.h"
//#include "Scenes/Scene.h"
#include "Scenes/TitleScene.h"

namespace Sokoban
{
	Game::Game() = default;
	
	Game::~Game() = default;

	void Game::Setup()
	{
		m_window = std::make_unique<Dive::Window>(800, 600, L"SOKOBAN");
		m_graphics = std::make_unique<Dive::Graphics>(m_window.get());
		m_gui = std::make_unique<Dive::ImGuiManager>(m_graphics.get());
		m_input = std::make_unique<Dive::Input>(m_window->GetWindowHandle());
		m_timer = std::make_unique<Dive::Timer>();

		m_window->GetWindowEvent().Add([&](const Dive::WindowEventData& data) { OnWindowEvent(data); });
	}

	void Game::Start()
	{
		GameContext context;
		context.game = this;
		context.window = m_window.get();
		context.graphics = m_graphics.get();
		context.input = m_input.get();
		context.changeScene = [this](eSceneType next) { this->ChangeScene(next); };

		m_currentScene = std::make_unique<TitleScene>(context);
	}

	void Game::Stop()
	{

	}

	int Game::Run()
	{
		Setup();

		{
			// 기존에는 여기에서 Engine객체들의 올바른 초기화를 검사했다.
		}

		Start();

		while (!m_window->ShouldClose())
		{
			m_window->PollEvents();
			m_timer->Tick();
			m_input->Update();

			if (m_currentScene) m_currentScene->Update(m_timer->GetDeltaTime());

			// m_gameRenderTarget->Bind(); // 게임용 렌더타겟으로 전환
			// m_gameRenderTarget->Clear();
			 if (m_currentScene) m_currentScene->Render();
			// m_gameRenderTarget->Unbind(); // 원래 백버퍼로 복구

			m_graphics->ClearBackbuffer();
			m_graphics->SetBackbuffer();

			m_gui->Begin();
			{
				if (m_currentScene) m_currentScene->RenderUI();
			}
			m_gui->End();

			m_graphics->SwapBuffers();
		}

		Stop();

		return 0;
	}

	void Game::ChangeScene(eSceneType type)
	{
		m_currentScene.reset();

		GameContext context;
		context.game = this;
		context.window = m_window.get();
		context.graphics = m_graphics.get();
		context.input = m_input.get();
		context.changeScene = [this](eSceneType next) { this->ChangeScene(next); };

		switch (type)
		{
		case eSceneType::Title:
			m_currentScene = std::make_unique<TitleScene>(context);
		}

		m_currentScene->Initialize();
	}

	void Game::OnWindowEvent(const Dive::WindowEventData& data)
	{
		m_gui->HandleWindowMessage(data);
	}
}