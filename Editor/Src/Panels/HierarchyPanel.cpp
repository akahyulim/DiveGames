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
        //auto rootGameObjects = m_pActiveScene->Get();
        //for (auto pObject : rootGameObjects)
        {
        //   showAddedTree(pObject);
        }

        ImGui::TreePop();
    }
}
