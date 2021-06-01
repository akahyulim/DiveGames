#include "Widget_Scene.h"

// Renderer�κ��� Image�� ������ ����Ѵ�.
namespace Editor
{
    Widget_Scene::Widget_Scene(Editor* pEditor)
        : Widget(pEditor)
    {
        m_title = "Scene";
        // ũ�� ������ �ǹ̰� �ֳ� �ʹ�.
        m_flags |= ImGuiWindowFlags_NoScrollbar;
        m_padding = ImVec2(4.0f, 4.0f);
    }

    void Widget_Scene::TickVisible()
    {
    }
}