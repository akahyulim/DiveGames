#include "InspectorView.h"

namespace Dive
{
	GameObject* InspectorView::s_InspectedGameObject = nullptr;

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
		if (!s_InspectedGameObject)
			return;
	}

	void InspectorView::Inspect(GameObject* gameObject)
	{
		s_InspectedGameObject = gameObject;
	}
}