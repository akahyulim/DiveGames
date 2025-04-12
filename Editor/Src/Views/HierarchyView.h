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

		void SetWorld(std::weak_ptr<World> world);
		void SetSelectedNode(GameObject node = {});

	protected:
		void drawView() override;

	private:
		void showNode(GameObject node);

	private:
		std::weak_ptr<World> m_World;
		GameObject m_SelectedNode;
	};
}
