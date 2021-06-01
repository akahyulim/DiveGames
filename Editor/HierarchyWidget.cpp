#include "HierarchyWidget.h"

// Scene�� �����ϴ� GameObject���� ���������� �����ش�.
namespace Editor
{
    HierarchyWidget::HierarchyWidget(Editor* pEditor)
        : Widget(pEditor)
    {
        m_title = "Hierarchy";
        m_flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }
}