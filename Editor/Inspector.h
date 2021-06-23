#pragma once
#include "Widget.h"

namespace editor
{
	class Inspector : public Widget
	{
	public:
		Inspector(Editor* editor);

		void TickVisible() override;
	private:
	private:
	};
}