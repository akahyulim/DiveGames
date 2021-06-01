#pragma once
#include "Widget.h"

namespace Editor
{
	class MenuBarWidget : public Widget
	{
	public:
		MenuBarWidget(Editor* pEditor);

		void TickAlways() override;
		float GetPadding() { return 8.0f; }

	private:
	private:
	};
}