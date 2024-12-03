#pragma once
#include "Dive.h"
#include "Panels/Menubar.h"
#include "Panels/WorldView.h"

namespace Dive
{
	class Editor
	{
	public:
		Editor();
		~Editor() = default;

		int Run();

		void OnPostRender();

	private:
		void initializeImGui();
		void renderImGui();

	private:
		std::unique_ptr<Engine> m_pEngine;

		std::unique_ptr<Menubar> m_pMenubar;
		std::unique_ptr<WorldView> m_pWorldView;
	};
}