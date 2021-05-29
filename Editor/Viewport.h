#pragma once
#include "Widget.h"

namespace Editor
{
	class Viewport : public Widget
	{
	public:
		Viewport(Editor* pEditor);

		void TickVisible() override;

	private:
	private:
	};
}