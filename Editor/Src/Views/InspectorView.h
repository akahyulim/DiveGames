#pragma once
#include "View.h"

namespace Dive
{
	class InspectorView : public View
	{
	public:
		InspectorView(Editor* editor);
		virtual ~InspectorView();

		void drawView() override;

		void SetSelectedNode(GameObject node) { m_SelectedNode = node; }

	private:
		GameObject m_SelectedNode;
	};
}
