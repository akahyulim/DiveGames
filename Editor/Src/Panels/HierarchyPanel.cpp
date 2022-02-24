#include "HierarchyPanel.h"
#include "Base/EditorScene.h"

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
	{
		m_pSelectedObject = nullptr;
		return;
	}

	if (ImGui::TreeNodeEx(m_pActiveScene->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
			{
				auto pInstanceID = static_cast<unsigned long long*>(pPayload->Data);
				auto pDroppedObject = m_pActiveScene->GetGameObject(*pInstanceID);
				pDroppedObject->GetComponent<Dive::Transform>()->SetParent(nullptr);
			}

			ImGui::EndDragDropTarget();
		}

		auto pRoots = m_pActiveScene->GetRoots();
		if (!pRoots.empty())
		{
			for (auto pRoot : pRoots)
			{
				drawNode(pRoot);
			}
		}

		ImGui::TreePop();
	}
}

void HierarchyPanel::drawNode(Dive::GameObject* pObject)
{
	if (!pObject)
		return;

	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;
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

	if (ImGui::BeginDragDropSource())
	{
		unsigned long long id = pObject->GetInstanceID();
		ImGui::SetDragDropPayload("HIERARCHY_NODE", &id, sizeof(unsigned long long));
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
		{
			auto pId = static_cast<unsigned long long*>(pPayload->Data);
			auto pDroppedObject = m_pActiveScene->GetGameObject(*pId);

			if (pDroppedObject->GetInstanceID() != pObject->GetInstanceID())
			{
				pDroppedObject->GetComponent<Dive::Transform>()->SetParent(
					pObject->GetComponent<Dive::Transform>());
			}
		}

		ImGui::EndDragDropTarget();
	}

	bool bCopy = false;
	bool bRename = false;
	bool bDelete = false;
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Copy"))
			bCopy = true;

		if (ImGui::MenuItem("Rename"))
			bRename = true;

		if (ImGui::MenuItem("Delete"))
			bDelete = true;

		ImGui::EndPopup();
	}

	if (bCopy)
	{

	}

	if (bRename)
	{

	}

	if (bDelete)
	{
		m_pActiveScene->RemoveGameObject(pObject);
		if (m_pSelectedObject && (m_pSelectedObject->GetInstanceID() == pObject->GetInstanceID()))
			m_pSelectedObject = nullptr;
	}


	if (bNodeOpen)
	{
		for (auto pChild : children)
		{
			drawNode(pChild->GetGameObject());
		}

		ImGui::TreePop();
	}
}
