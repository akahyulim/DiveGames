#pragma once
#include "Widget.h"

namespace Editor
{
	class Hierarchy : public Widget
	{
	public:
		Hierarchy(Editor* pEditor);

		void TickVisible() override;

	private:
	private:
		Dive::Scene* m_pScene;
	};
}