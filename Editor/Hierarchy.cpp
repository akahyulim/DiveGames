#include "Hierarchy.h"

namespace Editor
{
    Hierarchy::Hierarchy(Editor* pEditor)
        : Widget(pEditor)
    {
        m_title = "Hierarchy";
        m_flags |= ImGuiWindowFlags_HorizontalScrollbar;
        m_pScene = &Dive::Scene::GetGlobalScene();
    }

    void Hierarchy::TickVisible()
    {
    }

    // scene의 GameObject들을 계층구조로 보여준다.
    // GameObject의 생성, 제거를 적용한다.
    // 마우스 드래그를 통한 계층구조를 형성한다.
    // 마우스 조작을 통해 이름, 계층구조 변경, GameObject의 추가 및 제거를 구현한다.
    // 마지막으로 Scene을 파일화한다.

    // 드래그 적용을 위한 추가 구현이 필요하다.
    // Serialization의 대상은 GameObject와 Transform Component 정도가 될 것 같다.
}