#pragma once
#include "Widget.h"

namespace DiveEditor
{
	class MenuBar : public Widget
	{
	public:
		MenuBar(Editor* pEditor);

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