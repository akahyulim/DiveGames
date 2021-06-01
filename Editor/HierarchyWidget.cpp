#include "HierarchyWidget.h"

// Scene에 존재하는 GameObject들을 계층구조로 보여준다.
namespace Editor
{
    HierarchyWidget::HierarchyWidget(Editor* pEditor)
        : Widget(pEditor)
    {
        m_title = "Hierarchy";
        m_flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }
}