#pragma once
#include "Panel.h"

namespace Dive
{
	class MenuBarPanel : public Panel
	{
	public:
		MenuBarPanel(Editor* pEditor);

		void TickAlways() override;
		float GetPadding() { return 8.0f; }

	private:
		void modalNewProject();
		void modalNewScene();

	private:
		bool m_bModalNewProject;
		bool m_bModalNewScene;;
	};
}