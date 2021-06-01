#include "SceneWidget.h"

// Renderer�κ��� Image�� ������ ����Ѵ�.
namespace Editor
{
    SceneWidget::SceneWidget(Editor* pEditor)
        : Widget(pEditor)
    {
        m_title = "Scene";
        // ũ�� ������ �ǹ̰� �ֳ� �ʹ�.
        m_flags |= ImGuiWindowFlags_NoScrollbar;
        m_padding = ImVec2(4.0f, 4.0f);
    }

    void SceneWidget::TickVisible()
    {
    }
}