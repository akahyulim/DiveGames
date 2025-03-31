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
		// ���ĸ�ź�� ��� clicked�� hovered�� �����Ѵ�.
		// selected�� ��� camera���� �����Ѵ�.
		GameObject* m_SelectedGameObject;

		bool m_IsEditedName = false;
	};
}