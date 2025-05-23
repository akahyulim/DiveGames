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
	/*
	void HierarchyView::SetSelectedNode(Entity node)
	{
		if (m_SelectedNode != node)
			m_SelectedNode = node;

		m_Editor->GetWidget<InspectorView>()->SetSelectedNode(node);
	}
	*/
	void HierarchyView::SetSelectedNode(entt::entity node)
	{
		if (m_SelectedNode != node)
			m_SelectedNode = node;

		m_Editor->GetWidget<InspectorView>()->SetSelectedNode(node);
	}

	void HierarchyView::drawView()
	{
		if (!Engine::GetWorld())
			return;

		if (ImGui::TreeNodeEx(Engine::GetWorld()->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& entityManager = Engine::GetWorld()->GetEntityManager();

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
				{ 
					auto uuid = static_cast<UUID*>(payload->Data);
					auto draggedNode = entityManager.GetEntity(*uuid);
					entityManager.RemoveComponent<ParentComponent>(draggedNode);
				}

				ImGui::EndDragDropTarget();
			}

			auto rootNodes = entityManager.GetRootEntities();
			if (!rootNodes.empty())
			{
				for (auto& node : rootNodes)
				{
					showNode(node);
				}
			}

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				SetSelectedNode();

			ImGui::TreePop();
		}
	}
	
	void HierarchyView::showNode(entt::entity node)
	{
		auto& entityManager = Engine::GetWorld()->GetEntityManager();

		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;
		nodeFlags |=  (entityManager.HasComponent<Children>(node)) ?  ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Leaf;
		nodeFlags |= (m_SelectedNode == node) ? ImGuiTreeNodeFlags_Selected : 0;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)node, nodeFlags, entityManager.GetName(node).c_str());
		{
			if (ImGui::IsItemClicked())
				SetSelectedNode(node);

			if (ImGui::BeginDragDropSource())
			{
				auto uuid = entityManager.GetUUID(node);
				ImGui::SetDragDropPayload("HIERARCHY_NODE", &uuid, sizeof(UUID));
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
				{
					auto uuid = static_cast<UUID*>(payload->Data);
					auto draggedNode = entityManager.GetEntity(*uuid);

					if (draggedNode != node)
					{
						auto& pc = entityManager.HasComponent<ParentComponent>(draggedNode) ?
							entityManager.GetComponent<ParentComponent>(draggedNode) : entityManager.AddComponent<ParentComponent>(draggedNode);
						pc.Parent = node;
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
				if (entityManager.HasComponent<Children>(node))
				{
					auto& children = entityManager.GetComponent<Children>(node);
					for (auto& child : children.entities)
					{
						showNode(child);
					}
				}

				ImGui::TreePop();
			}

			if (nodeAdded)
			{
				auto addedNode = entityManager.CreateEntity("Entity");
				entityManager.AddComponent<ActiveComponent>(addedNode).IsActive = true;
				entityManager.AddComponent<LocalTransform>(addedNode);
			}

			if (nodeDeleted)
			{
				entityManager.DeleteEntity(node);
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
}
