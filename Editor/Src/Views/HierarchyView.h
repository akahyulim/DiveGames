#pragma once
#include "View.h"

namespace Dive
{
	class HierarchyView : public View
	{
	public:
		HierarchyView(Editor* editor);
		virtual ~HierarchyView();

		void drawView() override;

	protected:
		void showTree();
		void addObjectToTree(GameObject* gameObject);
		
		void setSelectedObject(GameObject* gameObject);


	private:
		// 스파르탄의 경우 clicked와 hovered만 관리한다.
		// selected의 경우 camera에서 관리한다.
		GameObject* m_SelectedGameObject;

		bool m_IsEditedName = false;
	};
}
