#include "Inspector.h"

// ���� ���õ� GameObject�� Inspector���� �����ش�.
namespace Editor
{
	Inspector::Inspector(Editor* pEditor)
		: Widget(pEditor)
	{
		m_title = "Inspector";
	}
}