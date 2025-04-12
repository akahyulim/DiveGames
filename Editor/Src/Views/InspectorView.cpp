#include "InspectorView.h"

namespace Dive
{
	InspectorView::InspectorView(Editor* editor)
		: View(editor)
	{
		m_Title = "Inspector";
	}

	InspectorView::~InspectorView()
	{
	}
	
	void InspectorView::drawView()
	{
		if (m_SelectedNode == GameObject{})
			return;

		if (ImGui::Button("Add Component"))
		{

		}
	}
}
