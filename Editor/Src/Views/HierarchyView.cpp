#include "HierarchyView.h"
#include "InspectorView.h"

namespace Dive
{
	HierarchyView::HierarchyView(Editor* editor)
		: View(editor)
		, m_SelectedGameObject(nullptr)
	{
		m_Title = "Hierarchy";
		m_Flags |= ImGuiWindowFlags_HorizontalScrollbar;
	}

	HierarchyView::~HierarchyView()
	{
	}

	void HierarchyView::drawView()
	{	
		if (!WorldManager::GetActiveWorld())
			return;

		showTree();

		if (ImGui::IsMouseReleased(0))
		{
			// clicekd와 hovered를 비교한 후 동일하다면 
			// SetSelectedObject()에 전달한다.
			// 이는 카메라에 SelectedObject()를 설정하는 과정이다.
			setSelectedObject(m_SelectedGameObject);
		}
	}

	void HierarchyView::showTree()
	{
		// 일단 에디터 모드가 아니면 ImGui::BeginDisabled(), EndDisabled()로 비활성화 한다.
		std::vector<GameObject*> rootObjects = WorldManager::GetActiveWorld()->GetRootGameObjects();
		for (auto gameObject : rootObjects)
		{
			if (gameObject->IsActive())
				addObjectToTree(gameObject);
		}
	}

	// 이 곳에서 설정하고 사용 중인 m_SelectedGameObject는 구버전 구현이다.
	// 새로운 버전을 이해한 후 적용하자.
	void HierarchyView::addObjectToTree(GameObject* gameObject)
	{
		if (!gameObject || gameObject->GetTag() == "Editor_Only")
			return;

		auto activeWorld = WorldManager::GetActiveWorld();	// 차라리 매개변수로 받는 편이 나을 것 같다.
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;

		std::vector<Dive::Transform*> children;
		if (gameObject->HasComponent<Dive::Transform>())
			children = gameObject->GetComponent<Dive::Transform>()->GetChildren();

		children.empty() ? nodeFlags |= ImGuiTreeNodeFlags_Leaf : nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
		if (m_SelectedGameObject)
			nodeFlags |= (m_SelectedGameObject == gameObject) ? ImGuiTreeNodeFlags_Selected : 0;

		// 각 노드 사이에 드랍 가능한 영역 추가
		ImGui::PushID(static_cast<int>(gameObject->GetID())); // 고유 ID 푸시
		ImGui::Selectable("##drop_target", false, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, 2.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
			{
				auto pId = static_cast<unsigned long long*>(payload->Data);
				auto droppedGameObject = activeWorld->GetGameObjectByID(*pId);

				if (droppedGameObject && droppedGameObject != gameObject)
				{
					if (droppedGameObject->HasComponent<Dive::Transform>())
						droppedGameObject->GetComponent<Dive::Transform>()->SetParent(nullptr);
				}
			}

			ImGui::EndDragDropTarget();
		}

		bool bNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)gameObject->GetID(), nodeFlags, gameObject->GetName().c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectedGameObject = gameObject;
		}

		if (ImGui::BeginDragDropSource())
		{
			auto id = gameObject->GetID();
			ImGui::SetDragDropPayload("HIERARCHY_NODE", &id, sizeof(unsigned long long));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
			{
				auto pId = static_cast<unsigned long long*>(payload->Data);
				auto droppedGameObject = activeWorld->GetGameObjectByID(*pId);

				if (droppedGameObject != gameObject)
				{
					if (droppedGameObject->HasComponent<Dive::Transform>())
					{
						droppedGameObject->GetComponent<Dive::Transform>()->SetParent(
							gameObject->GetComponent<Dive::Transform>());
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy"))
			{
			}

			if (ImGui::MenuItem("Rename"))
			{
				m_IsEditedName = true;
			}

			if (ImGui::MenuItem("Delete"))
			{
				// 현재 RemoveGameObject는 대상을 제거대상으로 마킹할 뿐이다.
				// 실제 제거는 다음 Update에서 수행한다.
				//activeWorld->RemoveGameObject(gameObject);
				activeWorld->DeleteGameObject(gameObject);
				if (m_SelectedGameObject && (m_SelectedGameObject == gameObject))
					m_SelectedGameObject = nullptr;
			}

			ImGui::EndPopup();
		}

		if (bNodeOpen)
		{
			if (m_SelectedGameObject == gameObject && m_IsEditedName)
			{
				ImGui::SetKeyboardFocusHere();

				char newName[256]{};
				ImGui::InputText("##edit", newName, sizeof(newName));

				if (ImGui::IsKeyPressed(ImGuiKey_Enter))
				{
					gameObject->SetName(newName);
					m_IsEditedName = false;
				}
			}
			for (auto childe : children)
				addObjectToTree(childe->GetGameObject());

			ImGui::TreePop();
		}

		ImGui::PopID(); // 고유 ID 팝
	}

	void HierarchyView::setSelectedObject(GameObject* gameObject)
	{
		// editor camera에서 select로 설정한다.

		InspectorView::Inspect(gameObject);
	}

}