#pragma once
#include "Widget.h"

namespace editor
{
	class Scene : public Widget
	{
	public:
		Scene(Editor* pEditor);

		void TickVisible() override;

	private:
	private:
		dive::Scene* mpScene;
		dive::Renderer* mpRenderer;
	};
}