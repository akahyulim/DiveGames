#include "Widget_Hierarchy.h"

// Scene�� �����ϴ� GameObject���� ���������� �����ش�.
namespace Editor
{
    Widget_Hierarchy::Widget_Hierarchy(Editor* pEditor)
        : Widget(pEditor)
    {
        m_title = "Hierarchy";
        m_flags |= ImGuiWindowFlags_HorizontalScrollbar;
        m_pScene = &Dive::Scene::GetGlobalScene();
    }
}