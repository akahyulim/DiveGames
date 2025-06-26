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
		void drawView() override;

	private:
		void showTree(std::shared_ptr<GameObject> gameObject);
		void popupMenu();

	private:
	};
}
