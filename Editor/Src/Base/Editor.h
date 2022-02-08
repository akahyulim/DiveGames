#pragma once
#include "DiveEngine.h"

namespace Dive
{
	class AppWindow;

	struct WindowProps
	{
		std::string Title	= "DiveEditor";
		unsigned int Width	= 1280;
		unsigned int Height = 780;
		bool Maximize		= false;
	};

	class Editor : public Engine
	{
	public:
		Editor(HINSTANCE hInstance);
		~Editor();

		void Initialize() override;
		void Destroy() override;

		void Tick() override;

		const WindowProps& GetWindowProps() const { return m_WindowProps; }

	private:
		void initializeImGui();
		void destroyImGui();

	private:
		AppWindow* m_pAppWnd = nullptr;

		WindowProps m_WindowProps;
	};
}