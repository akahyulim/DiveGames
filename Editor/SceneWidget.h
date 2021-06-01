#pragma once
#include "Widget.h"

namespace Editor
{
	class SceneWidget : public Widget
	{
	public:
		SceneWidget(Editor* pEditor);

		void TickVisible() override;

	private:
	private:
	};
}