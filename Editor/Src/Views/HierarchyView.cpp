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
		auto& editorContext = EditorContext::GetInstance();
		if (!editorContext.ActiveWorld)
			return;

		if (ImGui::TreeNodeEx(editorContext.ActiveWorld->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& entityManager = editorContext.ActiveWorld->GetEntityManager();

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
				{ 
					auto instanceID = static_cast<InstanceID*>(payload->Data);
					auto draggedNode = entityManager.GetEntity(*instanceID);
					entityManager.RemoveComponent<ParentComponent>(draggedNode);
				}

				ImGui::EndDragDropTarget();
			}

			auto rootNodes = entityManager.GetRootEntities();
			if (!rootNodes.empty())
			{
				for (auto& node : rootNodes)
				{
					if (entityManager.HasComponent<TagComponent>(node) &&
						entityManager.GetComponent<TagComponent>(node).Tag == TagComponent::eTag::EditorOnly)
						continue;
				
					showNode(node);
				}
			}

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				editorContext.Selected = entt::null;
			}

			ImGui::TreePop();
		}
	}
	
	void HierarchyView::showNode(entt::entity node)
	{
		if (node == entt::null)
			return;

		auto& editorContext = EditorContext::GetInstance();
		auto& entityManager = editorContext.ActiveWorld->GetEntityManager();

		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;
		nodeFlags |=  (entityManager.HasComponent<Children>(node)) ?  ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Leaf;
		nodeFlags |= (editorContext.Selected == node) ? ImGuiTreeNodeFlags_Selected : 0;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)node, nodeFlags, entityManager.GetName(node).c_str());
		{
			if (ImGui::IsItemClicked())
			{
				editorContext.Selected = node;
			}

			if (ImGui::BeginDragDropSource())
			{
				auto instanceID = entityManager.GetInstanceID(node);
				ImGui::SetDragDropPayload("HIERARCHY_NODE", &instanceID, sizeof(InstanceID));
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
				{
					auto instanceID = static_cast<InstanceID*>(payload->Data);
					auto draggedNode = entityManager.GetEntity(*instanceID);

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
				editorContext.Selected = entt::null;
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
