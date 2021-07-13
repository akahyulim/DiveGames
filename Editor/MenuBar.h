#pragma once
#include "Widget.h"

namespace editor
{
	class MenuBar : public Widget
	{
	public:
		MenuBar(Editor* editor);

		void TickAlways() override;
		float GetPadding() { return 8.0f; }

		void OnSetActiveScene();

	private:
		void modalNewScene();

	private:
		dive::Scene* m_Scene;
		bool m_bModalNewScene;
	};
}