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

	auto pRoots = m_pActiveScene->GetRoots();
	
	if (!pRoots.empty())
	{
		for (auto pRoot : pRoots)
		{
			drawNode(pRoot);
		}
	}
}

void HierarchyPanel::drawNode(Dive::GameObject* pObject)
{
	if (!pObject)
		return;

	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;

	// 자식 존재 여부
	auto children = pObject->GetComponent<Dive::Transform>()->GetChildren();
	children.empty() ? nodeFlags |= ImGuiTreeNodeFlags_Leaf : nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

	if (m_pSelectedObject)
	{
		nodeFlags |= (m_pSelectedObject->GetInstanceID() == pObject->GetInstanceID()) ? ImGuiTreeNodeFlags_Selected : 0;
	}

	bool bNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)pObject->GetInstanceID(), nodeFlags, pObject->GetName().c_str());

	if (ImGui::IsItemClicked())
	{
		m_pSelectedObject = pObject;
	}

	bool entityDeleted = false;
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Delete Entity"))
			entityDeleted = true;

		ImGui::EndPopup();
	}

	if (bNodeOpen)
	{
		for (auto pChild : children)
		{
			drawNode(pChild->GetGameObject());
		}

		ImGui::TreePop();
	}

	if (entityDeleted)
	{
		// 아직 미구현
		m_pActiveScene->RemoveGameObject(pObject);
		if (m_pSelectedObject && (m_pSelectedObject->GetInstanceID() == pObject->GetInstanceID()))
			m_pSelectedObject = nullptr;
	}
}
