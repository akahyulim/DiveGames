#pragma once
#include <memory>

namespace Dive
{
	class Window;
	class Graphics;
	class Input;

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	protected:
		virtual void Initialize() = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void Render() = 0;
		virtual void Shutdown() = 0;

		Graphics* GetGraphics() { return m_graphics.get(); }
		Input* GetInput() { return m_input.get(); }

	private:
		std::unique_ptr<Window> m_window;
		std::unique_ptr<Graphics> m_graphics;
		std::unique_ptr<Input> m_input;
	};
}