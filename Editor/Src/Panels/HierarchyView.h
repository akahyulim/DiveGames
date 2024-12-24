#pragma once
#include "Panel.h"

namespace Dive
{
	class HierarchyView : public Panel
	{
	public:
		HierarchyView(Editor* pEditor);
		virtual ~HierarchyView();

		void renderView() override;

	private:
		void drawNode(GameObject* pObject);

	private:
		std::shared_ptr<World> m_pActiveWorld;
		GameObject* m_pSelectedObject;
	};
}