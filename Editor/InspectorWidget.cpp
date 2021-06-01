#include "InspectorWidget.h"

// 현재 선택된 GameObject의 InspectorWidget들을 보여준다.
namespace Editor
{
	InspectorWidget::InspectorWidget(Editor* pEditor)
		: Widget(pEditor)
	{
		m_title = "Inspector";
	}
}