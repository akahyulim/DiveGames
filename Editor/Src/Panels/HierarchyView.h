#pragma once
#include "Panel.h"

namespace Dive
{
	class HierarchyView : public Panel
	{
	public:
		HierarchyView(Editor* pEditor);
		virtual ~HierarchyView();

		void drawView() override;

	private:
		void drawNode(GameObject* pObject);

	private:
		GameObject* m_pSelectedObject;
	};
}