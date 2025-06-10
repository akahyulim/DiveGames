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

	protected:
		void drawView() override;

	private:
		void showNode(entt::entity node);

	private:
	};
}
