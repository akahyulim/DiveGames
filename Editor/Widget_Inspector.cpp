#include "Widget_Inspector.h"

// 현재 선택된 GameObject의 Widget_Inspector들을 보여준다.
namespace Editor
{
	Widget_Inspector::Widget_Inspector(Editor* pEditor)
		: Widget(pEditor)
	{
		m_title = "Inspector";
	}
}