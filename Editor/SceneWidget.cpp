#include "SceneWidget.h"

// Renderer로부터 Image를 가져와 출력한다.
namespace Editor
{
    SceneWidget::SceneWidget(Editor* pEditor)
        : Widget(pEditor)
    {
        m_title = "Scene";
        // 크기 설정에 의미가 있나 싶다.
        m_flags |= ImGuiWindowFlags_NoScrollbar;
        m_padding = ImVec2(4.0f, 4.0f);
    }

    void SceneWidget::TickVisible()
    {
    }
}