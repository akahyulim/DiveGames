#pragma once
#include "Widget.h"

namespace Editor
{
	class Widget_Hierarchy : public Widget
	{
	public:
		Widget_Hierarchy(Editor* pEditor);

	private:
	private:
		Dive::Scene* m_pScene;
	};
}