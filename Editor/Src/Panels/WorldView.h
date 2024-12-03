#pragma once
#include "Panel.h"

namespace Dive
{
	class WorldView : public Panel
	{
	public:
		WorldView(Editor* pEditor);
		~WorldView() = default;

		void renderWindow() override;

	private:
	private:
	};
}