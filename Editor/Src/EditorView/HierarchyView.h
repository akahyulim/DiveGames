#pragma once
#include "EditorView.h"

namespace Dive
{
	class HierarchyView : public EditorView
	{
	public:
		HierarchyView();
		virtual ~HierarchyView();

		void drawView() override;

		GameObject* GetSelectedObject() { return m_pSelectedObject; }

	private:
		void drawNode(GameObject* pObject);

	private:
		GameObject* m_pSelectedObject;

		bool m_bEditName = false;
	};
}
