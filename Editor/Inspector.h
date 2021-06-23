#pragma once
#include "Widget.h"

namespace editor
{
	class Inspector : public Widget
	{
	public:
		Inspector(Editor* pEditor);

		void TickVisible() override;
	private:
	private:
	};
}