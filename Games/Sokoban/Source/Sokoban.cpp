#include "Sokoban.h"
#include "Core/LogManager.h"
#include "Core/Window.h"
#include "Graphics/Graphics.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/TextureManager.h"
#include "Rendering/Renderer2D.h"
#include "ImGui/ImGuiManager.h"
#include "Input/Input.h"
#include "Utilities/Timer.h"
#include "Scenes/TitleScene.h"
#include "Scenes/PlayScene.h"
#include "Scenes/EditorScene.h"
#include "Scenes/ScoreBoardScene.h"

namespace Sokoban
{
	Sokoban::Sokoban() = default;
	
	Sokoban::~Sokoban() = default;

	void Sokoban::Setup()
	{
		Dive::LogManager::SetFilename("sokoban.log");
		DV_LOG(Sokoban, info, "Sokoban Application Setup Started");

		m_window = std::make_unique<Dive::Window>(1600, 900, L"SOKOBAN");
		m_graphics = std::make_unique<Dive::Graphics>(m_window.get());
		Dive::TextureManager::Get().Initialize(m_graphics.get());
		m_gui = std::make_unique<Dive::ImGuiManager>(m_graphics.get());
		m_renderer = std::make_unique<Dive::Renderer2D>(m_graphics.get());
		m_input = std::make_unique<Dive::Input>(m_window->GetWindowHandle());
		m_timer = std::make_unique<Dive::Timer>();

		m_window->GetWindowEvent().Add([&](const Dive::WindowEventData& data) { OnWindowEvent(data); });

		DV_LOG(Sokoban, info, "Sokoban Application Setup Completed");
	}

	void Sokoban::Start()
	{
		m_camera.SetPerspective(DirectX::XM_PIDIV4, 1280.0f / 720.0f, 0.1f, 100.0f);
		m_camera.LookAt({ 0.0f, 30.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f });

		GameContext context;
		context.game = this;
		context.camera = &m_camera;
		context.window = m_window.get();
		context.graphics = m_graphics.get();
		context.renderer = m_renderer.get();
		context.input = m_input.get();
		context.changeScene = [this](eSceneType next) { this->ChangeScene(next); };

		m_currentScene = std::make_unique<TitleScene>(context);
		m_currentScene->Initialize();
	}

	void Sokoban::Stop()
	{
		DV_LOG(Sokoban, info, "Shutting down DiveLib Subsystems...");

		if (m_currentScene)
		{
			m_currentScene->Cleanup();
			m_currentScene.reset();
		}

		if (m_gui)
		{
			m_gui.reset();
		}

		if (m_input)
		{
			m_input.reset();
		}

		if (m_renderer)
		{
			m_renderer.reset();
		}

		if (m_graphics)
		{
			m_graphics.reset();
		}

		if (m_window)
		{
			m_window.reset();
		}

		DV_LOG(Sokoban, info, "All DiveLib Subsystems Shut Down Successfully.");

		Dive::LogManager::Shutdown();
	}

	int Sokoban::Run()
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

			ProcessSceneChange();

			if (m_currentScene) m_currentScene->Update(m_timer->GetDeltaTime());
			// 현재 Scene마다 각각의 rt를 사용하고 있다.
			// 이는 하나를 돌려쓰도록 이 곳에서 Begin/End로 감싸야한다.
			if (m_currentScene) m_currentScene->Render();

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

	void Sokoban::ChangeScene(eSceneType type)
	{
		m_nextSceneType = type;
		m_isSceneChangeRequested = true;
	}

	// Scene의 교체를 지연 수행
	void Sokoban::ProcessSceneChange()
	{
		if (!m_isSceneChangeRequested)
			return;

		m_currentScene.reset();

		GameContext context;
		context.game = this;
		context.camera = &m_camera;
		context.window = m_window.get();
		context.graphics = m_graphics.get();
		context.renderer = m_renderer.get();
		context.input = m_input.get();
		context.changeScene = [this](eSceneType next) { this->ChangeScene(next); };

		switch (m_nextSceneType)
		{
		case eSceneType::Title:
			m_currentScene = std::make_unique<TitleScene>(context);
			break;
		case eSceneType::Play:
			m_currentScene = std::make_unique<PlayScene>(context);
			break;
		case eSceneType::Editor:
			m_currentScene = std::make_unique<EditorScene>(context);
			break;
		case eSceneType::ScoreBoard:
			m_currentScene = std::make_unique<ScoreBoardScene>(context);
			break;
		default:
			assert(false && "잘못된 타입 전달!!!");
			break;
		}

		m_currentScene->Initialize();

		m_nextSceneType = eSceneType::None;
		m_isSceneChangeRequested = false;
	}

	void Sokoban::OnWindowEvent(const Dive::WindowEventData& data)
	{
		m_gui->HandleWindowMessage(data);
	}
}