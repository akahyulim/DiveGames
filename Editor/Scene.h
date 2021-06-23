#pragma once
#include "Widget.h"

namespace editor
{
	class Scene : public Widget
	{
	public:
		Scene(Editor* editor);

		void TickVisible() override;

	private:
	private:
		dive::Scene* mScene;
		dive::Renderer* mRenderer;
	};
}