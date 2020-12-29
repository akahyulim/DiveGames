#include "Hierarchy.h"


Hierarchy::Hierarchy(Editor * editor)
	: Widget(editor)
{
	m_scene = m_editor->GetContext()->GetSubsystem<Scene>().get();
}

void Hierarchy::Tick(float deltaTime)
{
	showTree();
}

void Hierarchy::showTree()
{
}
