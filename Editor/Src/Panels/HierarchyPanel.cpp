#include "HierarchyPanel.h"

HierarchyPanel::HierarchyPanel(Editor* pEditor)
	: Panel(pEditor, "Hierarchy")
{
}

HierarchyPanel::~HierarchyPanel()
{
}

void HierarchyPanel::renderWindow()
{
	if (!m_pActiveScene)
		return;

    char sceneInfo[32] = { 0, };
    sprintf_s(sceneInfo, "%s - %d", m_pActiveScene->GetName().c_str(), m_pActiveScene->GetGameObjectCount());

    if (ImGui::TreeNodeEx(sceneInfo, ImGuiTreeNodeFlags_DefaultOpen))
    {
        // 계층구조 때문에 좀 복잡하다.

        ImGui::TreePop();
    }
}
