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
	void HierarchyView::SetSelectedNode(GameObject node)
	{
		if (m_SelectedNode != node)
			m_SelectedNode = node;

		m_Editor->GetWidget<InspectorView>()->SetSelectedNode(node);
	}

	void HierarchyView::drawView()
	{
		if (Engine::GetActiveWorld())
		{
			if (ImGui::TreeNodeEx(Engine::GetActiveWorld()->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				// 일단 이 곳이랑 가장 아래 부분에서
				// Transform 참조 오류가 발생한다.
				// 현재 Transform이 없는 GameObject를 생성토록 하였으므로
				// 이는 반드시 수정되어야 한다.
				// => 수정했지만 여전히 버그가 발생한다. 다른 곳에서 SelectedObject의 Transform을 요구하는 것 같다.
				// => ScenePanel에서 ImGuizmo 때문에 발생한 문제였다. 일단 수정 및 테스트를 완료했다.
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
					{
						auto pInstanceID = static_cast<unsigned long long*>(pPayload->Data);
						auto droppedNode = Engine::GetActiveWorld()->GetGameObjectByUUID(*pInstanceID);
						if (droppedNode.HasComponent<TransformComponent>())
							Transforms::SetParent(droppedNode, GameObject{});
					}

					ImGui::EndDragDropTarget();
				}

				//auto pRoots = m_pActiveScene->GetRoots();
				auto rootNodes = Transforms::GetRootNodes(Engine::GetActiveWorld()->GetAllGameObjects());
				if (!rootNodes.empty())
				{
					for (auto& node : rootNodes)
					{
						if(node.GetTag() != eTag::EditorOnly)
							showNode(node);
					}
				}

				if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
					SetSelectedNode();

				ImGui::TreePop();
			}
		}
		/*
		if (Engine::GetActiveWorld())
		{
			// World 노드로 드래그 앤 드랍하면 부모를 없애고 싶었으나 현재 되지 않는다.
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;
			auto rootNodes = Transforms::GetRootNodes(Engine::GetActiveWorld()->GetAllGameObjects());
			rootNodes.empty() ? nodeFlags |= ImGuiTreeNodeFlags_Leaf : nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
			
			bool opened = ImGui::TreeNodeEx(Engine::GetActiveWorld()->GetName().c_str(), nodeFlags);
			if (opened)
			{
				for (auto& node : rootNodes)
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
						Engine::GetActiveWorld()->CreateGameObject();

					ImGui::EndPopup();
				}
				//

				ImGui::TreePop();
			}
		}*/
	}

	void HierarchyView::showNode(GameObject node)
	{
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;

		auto children = Transforms::GetChildren(node);
		children.empty() ? nodeFlags |= ImGuiTreeNodeFlags_Leaf : nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
		nodeFlags |= (m_SelectedNode == node) ? ImGuiTreeNodeFlags_Selected : 0;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)node, nodeFlags, node.GetComponent<NameComponent>().Name.c_str());
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
				auto droppedNode = Engine::GetActiveWorld()->GetGameObjectByUUID(*id);

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
				Engine::GetActiveWorld()->CreateGameObject(),
				m_SelectedNode);
		}

		if (nodeDeleted)
		{
			Engine::GetActiveWorld()->RemoveGameObject(node);
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
