#include "Inspector.h"

// 현재 선택된 GameObject의 Inspector들을 보여준다.
namespace Editor
{
	Inspector::Inspector(Editor* pEditor)
		: Widget(pEditor)
	{
		m_title = "Inspector";
	}
}