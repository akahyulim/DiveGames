#include "Viewport.h"


namespace Editor
{
    Viewport::Viewport(Editor* pEditor)
        : Widget(pEditor)
    {
        m_title = "Viewport";
        // ũ�� ������ �ǹ̰� �ֳ� �ʹ�.
        m_flags |= ImGuiWindowFlags_NoScrollbar;
        m_padding = ImVec2(4.0f, 4.0f);
    }

    void Viewport::TickVisible()
    {
        // ShaderResourceView�� ���ͼ� �׸���.
    }
}