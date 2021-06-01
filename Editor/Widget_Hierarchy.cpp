#include "Widget_Hierarchy.h"

// Scene에 존재하는 GameObject들을 계층구조로 보여준다.
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