#include "InspectorView.h"

namespace Dive
{
	InspectorView::InspectorView(Editor* pEditor)
		: Panel(pEditor)
	{
		m_Title = "Inspector";
	}

	InspectorView::~InspectorView()
	{
	}
}