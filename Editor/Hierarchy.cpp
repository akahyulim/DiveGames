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

    // scene�� GameObject���� ���������� �����ش�.
    // GameObject�� ����, ���Ÿ� �����Ѵ�.
    // ���콺 �巡�׸� ���� ���������� �����Ѵ�.
    // ���콺 ������ ���� �̸�, �������� ����, GameObject�� �߰� �� ���Ÿ� �����Ѵ�.
    // ���������� Scene�� ����ȭ�Ѵ�.

    // �巡�� ������ ���� �߰� ������ �ʿ��ϴ�.
    // Serialization�� ����� GameObject�� Transform Component ������ �� �� ����.
}