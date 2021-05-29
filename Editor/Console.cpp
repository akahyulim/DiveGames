#include "Console.h"

namespace Editor
{
	Console::Console(Editor* pEditor)
		: Widget(pEditor)
	{
		m_title = "Console";
	}
}