#include "Component.h"

namespace Editor
{
	Component::Component(Editor* pEditor)
		: Widget(pEditor)
	{
		m_title = "Component";
	}
}