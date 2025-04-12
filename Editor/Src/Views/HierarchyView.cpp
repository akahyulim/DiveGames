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

	void HierarchyView::SetWorld(std::weak_ptr<World> world)
	{
		m_World = world;
		SetSelectedNode();
	}

	void HierarchyView::SetSelectedNode(GameObject node)
	{
		if (m_SelectedNode != node)
			m_SelectedNode = node;

		m_Editor->GetWidget<InspectorView>()->SetSelectedNode(node);
	}

	void HierarchyView::drawView()
	{
		if (m_World.lock())
		{
			// World 노드로 드래그 앤 드랍하면 부모를 없애고 싶었으나 현재 되지 않는다.
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;
			auto rootEntities = Transforms::GetRootNodes(m_World.lock()->GetAllGameObjects());
			rootEntities.empty() ? nodeFlags |= ImGuiTreeNodeFlags_Leaf : nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
			
			bool opened = ImGui::TreeNodeEx(m_World.lock()->GetName().c_str(), nodeFlags);
			if (opened)
			{
				for (auto& node : rootEntities)
				{
					if (node.GetComponent<ActiveComponent>().IsActive)
						showNode(node);
				}

				if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
					SetSelectedNode();

				// 추후에 추가
				/*
				if (ImGui::BeginPopupContextWindow())
				{
					if (ImGui::MenuItem("Add GameObject"))
						m_World.lock()->CreateGameObject();

					ImGui::EndPopup();
				}
				*/

				ImGui::TreePop();
			}
		}
	}

	void HierarchyView::showNode(GameObject node)
	{
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;

		auto children = Transforms::GetChildren(node);
		children.empty() ? nodeFlags |= ImGuiTreeNodeFlags_Leaf : nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
		nodeFlags |= (m_SelectedNode == node) ? ImGuiTreeNodeFlags_Selected : 0;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)node, nodeFlags, node.GetComponent<TagComponent>().Tag.c_str());
		if (ImGui::IsItemClicked())
			SetSelectedNode(node);

		if (ImGui::BeginDragDropSource())
		{
			auto id = node.GetUUID();
			ImGui::SetDragDropPayload("HIERARCHY_NODE", &id, sizeof(unsigned long long));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
			{
				auto id = static_cast<unsigned long long*>(payload->Data);
				auto droppedNode = m_World.lock()->GetGameObjectByUUID(*id);

				if (droppedNode != node)
				{
					if (droppedNode.HasComponent<TransformComponent>())
						Transforms::SetParent(droppedNode, node);
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
			for (auto& child : children)
			{
				showNode(child);
			}

			ImGui::TreePop();
		}

		if (nodeAdded)
		{
			Transforms::SetParent(
				m_World.lock()->CreateGameObject(),
				m_SelectedNode);
		}

		if (nodeDeleted)
		{
			m_World.lock()->RemoveGameObject(node);
			if (m_SelectedNode == node)
				SetSelectedNode();
		}

		if (nodeCopied)
		{
		}

		if (nodeRenamed)
		{

		}
	}
}
