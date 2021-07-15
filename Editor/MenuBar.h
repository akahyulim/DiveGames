#pragma once
#include "Widget.h"

namespace editor
{
	class MenuBar : public Widget
	{
	public:
		MenuBar(Editor* editor);

		void TickAlways() override;
		float GetPadding() { return 8.0f; }

	private:
		void modalNewScene();

	private:
		bool m_bModalNewScene;
	};
}