#include "InspectorWidget.h"

// ���� ���õ� GameObject�� InspectorWidget���� �����ش�.
namespace Editor
{
	InspectorWidget::InspectorWidget(Editor* pEditor)
		: Widget(pEditor)
	{
		m_title = "Inspector";
	}
}