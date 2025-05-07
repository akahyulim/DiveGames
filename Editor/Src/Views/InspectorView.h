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

		void SetSelectedNode(Entity node) { m_SelectedNode = node; }

	private:
		Entity m_SelectedNode;
	};
}
