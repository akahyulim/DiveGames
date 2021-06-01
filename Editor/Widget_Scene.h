#pragma once
#include "Widget.h"

namespace Editor
{
	class Widget_Scene : public Widget
	{
	public:
		Widget_Scene(Editor* pEditor);

		void TickVisible() override;

	private:
	private:
	};
}