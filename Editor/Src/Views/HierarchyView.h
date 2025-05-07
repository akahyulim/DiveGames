#pragma once
#include "View.h"

namespace Dive
{
	class World;

	class HierarchyView : public View
	{
	public:
		HierarchyView(Editor* editor);
		virtual ~HierarchyView();

		void SetSelectedNode(Entity node = {});

	protected:
		void drawView() override;

	private:
		void showNode(Entity node);

	private:
		Entity m_SelectedNode;
	};
}
