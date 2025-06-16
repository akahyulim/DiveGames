#include "HierarchyView.h"
#include "InspectorView.h"
#include "../Editor.h"

namespace Dive
{
	HierarchyView::HierarchyView(Editor* eidtor)
		: View(eidtor)
	{
		m_Title = "Hierarchy";
		m_Flags |= ImGuiWindowFlags_HorizontalScrollbar;
	}

	HierarchyView::~HierarchyView()
	{

	}

	void HierarchyView::drawView()
	{
		if (!EditorContext::ActiveWorld)
			return;

		if (ImGui::TreeNodeEx(EditorContext::ActiveWorld->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
				{ 
					auto instanceID = static_cast<InstanceID*>(payload->Data);
					auto draggedNode = EditorContext::ActiveWorld->GetGameObjectByInstanceID(*instanceID);
					draggedNode->GetTransform()->SetParent(nullptr);
				}

				ImGui::EndDragDropTarget();
			}

			auto rootNodes = EditorContext::ActiveWorld->GetRootGameObjects();
			if (!rootNodes.empty())
			{
				for (auto& node : rootNodes)
				{
					if (node->GetTag() == "EditorOnly")
						continue;

					showNode(node);
				}
			}

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				EditorContext::Selected = nullptr;
			}

			ImGui::TreePop();
		}
	}

	void HierarchyView::showNode(GameObject* node)
	{
		if (node == nullptr)
			return;

		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;
		nodeFlags |= node->GetComponent<Transform>()->HasChildren() ? ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Leaf;
		nodeFlags |= (EditorContext::Selected == node) ? ImGuiTreeNodeFlags_Selected : 0;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)node, nodeFlags, node->GetName().c_str());
		{
			if (ImGui::IsItemClicked())
			{
				EditorContext::Selected = node;
			}

			if (ImGui::BeginDragDropSource())
			{
				auto instanceID = node->GetInstanceID();
				ImGui::SetDragDropPayload("HIERARCHY_NODE", &instanceID, sizeof(InstanceID));
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
				{
					auto instanceID = static_cast<InstanceID*>(payload->Data);
					auto draggedNode = EditorContext::ActiveWorld->GetGameObjectByInstanceID(*instanceID);

					if (draggedNode != node)
					{
						draggedNode->GetTransform()->SetParent(node->GetTransform());
					}
				}

				ImGui::EndDragDropTarget();
			}

			bool nodeDeleted = false;
			bool nodeAdded = false;
			bool nodeCopied = false;
			bool nodeRenamed = false;
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Add Empty"))
					nodeAdded = true;

				if (ImGui::MenuItem("Delete"))
					nodeDeleted = true;

				if (ImGui::MenuItem("Copy"))
					nodeCopied = true;

				if (ImGui::MenuItem("Rename"))
					nodeRenamed = true;

				ImGui::EndPopup();
			}

			if (opened)
			{
				if(node->GetTransform()->HasChildren())
				{
					auto children = node->GetTransform()->GetChildren();
					for (auto& child : children)
					{
						showNode(child->GetGameObject());
					}
				}

				ImGui::TreePop();
			}

			if (nodeAdded)
			{
				auto addedNode = EditorContext::ActiveWorld->CreateGameObject("GameObject");
			}

			if (nodeDeleted)
			{
				EditorContext::ActiveWorld->DeleteGameObject(node);
				EditorContext::Selected = nullptr;
			}

			if (nodeCopied)
			{
			}

			if (nodeRenamed)
			{

			}
		}
	}
}
