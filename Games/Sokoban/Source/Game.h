#pragma once
#include <memory>
#include <vector>
#include <functional>

namespace Dive
{
	struct WindowEventData;
	class Window;
	class Graphics;
	class Input;
	class ImGuiManager;
	class Timer;
}

namespace Sokoban
{
	class Game;

	enum class eSceneType
	{
		Title,
		Play,
		Editor
	};

	struct GameContext
	{
		Game* game = nullptr;
		Dive::Window* window = nullptr;
		Dive::Graphics* graphics = nullptr;
		Dive::Input* input = nullptr;

		std::function<void(eSceneType)> changeScene;
	};

	class Scene;

	class Game
	{
	public:
		Game();
		~Game();

		void Setup();
		void Start();
		void Stop();

		int Run();

		void ChangeScene(eSceneType type);
		
		void OnWindowEvent(const Dive::WindowEventData& data);

	private:

	private:
		std::unique_ptr<Dive::Window> m_window;
		std::unique_ptr<Dive::Graphics> m_graphics;
		std::unique_ptr<Dive::ImGuiManager> m_gui;
		std::unique_ptr<Dive::Input> m_input;
		std::unique_ptr<Dive::Timer> m_timer;

		std::unique_ptr<Scene> m_currentScene;
	};
}