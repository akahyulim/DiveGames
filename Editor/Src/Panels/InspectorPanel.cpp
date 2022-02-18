#include "InspectorPanel.h"

InspectorPanel::InspectorPanel(Editor* pEditor)
	: Panel(pEditor, "Inspector")
{
}

InspectorPanel::~InspectorPanel()
{
}

void InspectorPanel::renderWindow()
{
	if (!m_pActiveScene)
		return;

	// 이건 selected를 알아야 한다.
}
