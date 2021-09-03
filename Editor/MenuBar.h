#pragma once
#include "Widget.h"

namespace Editor
{
	class MenuBar : public Widget
	{
	public:
		MenuBar(Editor* pEditor);

		void TickAlways() override;
		float GetPadding() { return 8.0f; }

	private:
		void modalNewScene();

	private:
		bool m_bModalNewScene;
	};
}