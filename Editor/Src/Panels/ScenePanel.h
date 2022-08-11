#pragma once
#include "Panel.h"

namespace Editor
{
	class ScenePanel : public Panel
	{
	public:
		ScenePanel(Editor* pEditor);
		~ScenePanel();

		void renderWindow() override;

	private:
	};
}