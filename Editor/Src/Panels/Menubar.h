#pragma once
#include "Panel.h"

namespace Dive
{
	class Menubar : public Panel
	{
	public:
		Menubar(Editor* pEditor);
		~Menubar() = default;

		void renderAlways() override;

	private:
		void menuFile();
		void menuGameObject();
		void menuComponent();
		void menuTools();
		void menuView();
		void menuHelp();

		void showNewProject();
		void showNewWorld();

		void showOpenProject();
		void showOpenWorld();

	private:
		bool m_bShowNewProject;
		bool m_bShowNewWorld;
		bool m_bShowOpenProject;
		bool m_bShowOpenWorld;
	};
}