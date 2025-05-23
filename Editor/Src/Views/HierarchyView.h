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

		//void SetSelectedNode(Entity node = {});
		void SetSelectedNode(entt::entity node = entt::null);

	protected:
		void drawView() override;

	private:
		//void showNode(Entity node);
		void showNode(entt::entity node);

	private:
		//Entity m_SelectedNode;
		entt::entity m_SelectedNode = entt::null;
	};
}
