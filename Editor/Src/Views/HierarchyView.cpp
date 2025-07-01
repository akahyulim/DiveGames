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

	void HierarchyView::drawView()
	{
		if (!EditorContext::ActiveWorld)
			return;

		popupMenu();

		if (ImGui::TreeNodeEx(EditorContext::ActiveWorld->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
				{ 
					auto draggedGO = static_cast<GameObject*>(payload->Data);
					draggedGO->GetTransform()->SetParent(nullptr);
				}

				ImGui::EndDragDropTarget();
			}

			// 이터레이터가 꼬이지 않게 얕은복사로 사용
			auto root = EditorContext::ActiveWorld->GetRootGameObjects();
			if (!root.empty())
			{
				for (auto gameObject : root)
				{
					if (gameObject->GetTag() == "EditorOnly")
						continue;

					showTree(gameObject);
				}
			}

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				EditorContext::Selected = nullptr;
			}

			ImGui::TreePop();
		}
	}

	void HierarchyView::showTree(GameObject* gameObject)
	{
		if (!gameObject) return;

		auto transform = gameObject->GetTransform();

		ImGui::PushID(gameObject);
		
		// 드롭 슬롯 (노드 위)
		float width = ImGui::GetContentRegionAvail().x;
		ImGui::InvisibleButton("DropSlotBefore", ImVec2(width, 4));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
			{
				auto draggedGO = static_cast<GameObject*>(payload->Data);
				if (draggedGO && draggedGO != gameObject)
				{
					auto draggedTransform = draggedGO->GetTransform();
					auto targetTransform = transform;
					auto parent = targetTransform->GetParent();

					if (draggedTransform != parent)
					{
						if (draggedTransform->GetParent() != parent)
							draggedTransform->SetParent(parent);

						auto insertIdx = targetTransform->GetSiblingIndex();
						draggedTransform->SetSiblingIndex(insertIdx);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_AllowItemOverlap;
		flags |= transform->HasChildren() ? ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Leaf;
		flags |= (EditorContext::Selected == gameObject) ? ImGuiTreeNodeFlags_Selected : 0;

		bool opened = ImGui::TreeNodeEx((void*)(intptr_t)gameObject, flags, gameObject->GetName().c_str());

		if (ImGui::BeginDragDropSource())
		{
			// 드래그 노드로 다룰 데이터의 포인터와 크기 설정
			ImGui::SetDragDropPayload("HIERARCHY_NODE", gameObject, sizeof(GameObject));
			ImGui::Text("Move %s", gameObject->GetName().c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
			{
				auto draggedGO = static_cast<GameObject*>(payload->Data);
				if (draggedGO && draggedGO != gameObject)
				{
					auto draggedTransform = draggedGO->GetTransform();
					if (!transform->IsChildOf(draggedTransform))
						draggedTransform->SetParent(transform);
				}
			}
			ImGui::EndDragDropTarget();
		}

		static bool isCopied = false;

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
			{
				EditorContext::ActiveWorld->DestroyGameObject(gameObject);
				EditorContext::Selected = nullptr;
			}
			if (ImGui::MenuItem("Copy"))
			{
				// 하위 게임오브젝트들까지 계층구조를 유지하여 복사된다.
				// 생각보다 좀 복잡하다.
				// 이걸 누른다고 바로 붙여넣기가 수행되는 게 아니다.
				isCopied = true;
			}
			if (ImGui::MenuItem("Paste", nullptr, nullptr, isCopied))
			{
				// 이건 메뉴 위치가 애매하다.
				// 현재 구현을 유지하려면 트리 아이템 위에서 이 메뉴를 선택해야 하고
				// 붙여넣기되는 지점도 가장 마지막 루트 노드여야 한다.
				// 이 팝업의 경우엔 Selected 하위에 붙여넣기 하고
				// AddGameObject 팝업에도 Paset 메뉴를 추가해 루트노드에 붙여넣는 방법도 있다.
			}

			ImGui::EndPopup();
		}

		if (ImGui::IsItemClicked())
			EditorContext::Selected = gameObject;

		if (opened)
		{
			// 역시 얕은 복사
			auto childrenCopy = transform->GetChildren();
			for (auto child : childrenCopy)
				showTree(child->GetGameObject());
			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	void HierarchyView::popupMenu()
	{
		if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
		{
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
				ImGui::OpenPopup("AddGameObject");
		}

		if (ImGui::BeginPopupContextWindow("AddGameObject", ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::MenuItem("Add Empty"))
				EditorContext::ActiveWorld->CreateGameObject("Empty");

			ImGui::Separator();

			if (ImGui::BeginMenu("3D Object"))
			{
				if (ImGui::MenuItem("Triangle", nullptr, nullptr))
				{
				}
				if (ImGui::MenuItem("Quad", nullptr, nullptr))
				{
				}

				if (ImGui::MenuItem("Plane", nullptr, nullptr))
				{
				}
				if (ImGui::MenuItem("Cube", nullptr, nullptr))
				{
				}
				if (ImGui::MenuItem("Sphere", nullptr, nullptr))
				{
				}
				if (ImGui::MenuItem("Capsule", nullptr, nullptr))
				{
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("Directional Light", nullptr, nullptr))
				{
				}
				if (ImGui::MenuItem("Point Light", nullptr, nullptr))
				{
				}
				if (ImGui::MenuItem("Spot Light", nullptr, nullptr))
				{

				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Camera"))
			{
				if (ImGui::MenuItem("Perpective", nullptr, nullptr))
				{
				}

				if (ImGui::MenuItem("Orthographic", nullptr, nullptr))
				{
				}
				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
	}
}
