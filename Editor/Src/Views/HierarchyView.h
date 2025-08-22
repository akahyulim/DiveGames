#pragma once
#include "View.h"

namespace Dive
{
	class World;

	class HierarchyView : public View
	{
	public:
		HierarchyView(Editor* editor);
		virtual ~HierarchyView() = default;

	protected:
		void renderContent() override;

	private:
		void showTree(GameObject* gameObject);
		void popupMenu();

	private:
	};
}
