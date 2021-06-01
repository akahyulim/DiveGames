#pragma once
#include "Widget.h"

namespace Editor
{
	class Widget_MenuBar : public Widget
	{
	public:
		Widget_MenuBar(Editor* pEditor);

		void TickAlways() override;
		float GetPadding() { return 8.0f; }

	private:
	private:
		Dive::Scene* m_pScene;
	};
}