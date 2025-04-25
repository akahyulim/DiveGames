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

		void SetSelectedNode(GameObject node = {});

	protected:
		void drawView() override;

	private:
		void showNode(GameObject node);

	private:
		GameObject m_SelectedNode;
	};
}
