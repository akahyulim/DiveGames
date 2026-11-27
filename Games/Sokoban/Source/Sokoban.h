#pragma once
#include <memory>
#include <vector>
#include <functional>
#include "Graphics/Camera.h"

namespace Dive
{
	struct WindowEventData;
	class Window;
	class Graphics;
	class Renderer2D;
	class Input;
	class ImGuiManager;
	class Timer;
}

namespace Sokoban
{
	class Sokoban;

	enum class eSceneType
	{
		None,
		Title,
		Play,
		Editor,
		ScoreBoard
	};

	struct GameContext
	{
		Sokoban* game = nullptr;

		Camera* camera = nullptr;

		Dive::Window* window = nullptr;
		Dive::Graphics* graphics = nullptr;
		Dive::Renderer2D* renderer = nullptr;
		Dive::Input* input = nullptr;

		std::function<void(eSceneType)> changeScene;
	};

	class Scene;

	class Sokoban
	{
	public:
		Sokoban();
		~Sokoban();

		void Setup();
		void Start();
		void Stop();

		int Run();

		void ChangeScene(eSceneType type);
		void ProcessSceneChange();
		
		void OnWindowEvent(const Dive::WindowEventData& data);

	private:

	private:
		std::unique_ptr<Dive::Window> m_window;
		std::unique_ptr<Dive::Graphics> m_graphics;
		std::unique_ptr<Dive::ImGuiManager> m_gui;
		std::unique_ptr<Dive::Renderer2D> m_renderer;
		std::unique_ptr<Dive::Input> m_input;
		std::unique_ptr<Dive::Timer> m_timer;

		std::unique_ptr<Scene> m_currentScene;
		
		eSceneType m_nextSceneType = eSceneType::None;
		bool m_isSceneChangeRequested = false;

		Camera m_camera;
	};
}