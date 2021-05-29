#include "Viewport.h"


namespace Editor
{
    Viewport::Viewport(Editor* pEditor)
        : Widget(pEditor)
    {
        m_title = "Viewport";
        // 크기 설정에 의미가 있나 싶다.
        m_flags |= ImGuiWindowFlags_NoScrollbar;
        m_padding = ImVec2(4.0f, 4.0f);
    }

    void Viewport::TickVisible()
    {
        // ShaderResourceView를 얻어와서 그린다.
    }
}