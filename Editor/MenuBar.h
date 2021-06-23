#pragma once
#include "Widget.h"

namespace editor
{
	class MenuBar : public Widget
	{
	public:
		MenuBar(Editor* pEditor);

		void TickAlways() override;
		float GetPadding() { return 8.0f; }

	private:
	private:
		dive::Scene* mpScene;
	};
}