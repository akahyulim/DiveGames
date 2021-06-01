#include "Scene.h"

// Renderer�κ��� Image�� ������ ����Ѵ�.
namespace Editor
{
    Scene::Scene(Editor* pEditor)
        : Widget(pEditor)
    {
        m_title = "Scene";
        // ũ�� ������ �ǹ̰� �ֳ� �ʹ�.
        m_flags |= ImGuiWindowFlags_NoScrollbar;
        m_padding = ImVec2(4.0f, 4.0f);
    }

    void Scene::TickVisible()
    {
    }
}