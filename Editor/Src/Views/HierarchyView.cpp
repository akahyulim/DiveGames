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
			// clicekd�� hovered�� ���� �� �����ϴٸ� 
			// SetSelectedObject()�� �����Ѵ�.
			// �̴� ī�޶� SelectedObject()�� �����ϴ� �����̴�.
			setSelectedObject(m_SelectedGameObject);
		}
	}

	void HierarchyView::showTree()
	{
		// �ϴ� ������ ��尡 �ƴϸ� ImGui::BeginDisabled(), EndDisabled()�� ��Ȱ��ȭ �Ѵ�.
		std::vector<GameObject*> rootObjects = WorldManager::GetActiveWorld()->GetRootGameObjects();
		for (auto gameObject : rootObjects)
		{
			if (gameObject->IsActive())
				addObjectToTree(gameObject);
		}
	}

	// �� ������ �����ϰ� ��� ���� m_SelectedGameObject�� ������ �����̴�.
	// ���ο� ������ ������ �� ��������.
	void HierarchyView::addObjectToTree(GameObject* gameObject)
	{
		if (!gameObject || gameObject->GetTag() == "Editor_Only")
			return;

		auto activeWorld = WorldManager::GetActiveWorld();	// ���� �Ű������� �޴� ���� ���� �� ����.
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;

		std::vector<Dive::Transform*> children;
		if (gameObject->HasComponent<Dive::Transform>())
			children = gameObject->GetComponent<Dive::Transform>()->GetChildren();

		children.empty() ? nodeFlags |= ImGuiTreeNodeFlags_Leaf : nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
		if (m_SelectedGameObject)
			nodeFlags |= (m_SelectedGameObject == gameObject) ? ImGuiTreeNodeFlags_Selected : 0;

		// �� ��� ���̿� ��� ������ ���� �߰�
		ImGui::PushID(static_cast<int>(gameObject->GetID())); // ���� ID Ǫ��
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
				// ���� RemoveGameObject�� ����� ���Ŵ������ ��ŷ�� ���̴�.
				// ���� ���Ŵ� ���� Update���� �����Ѵ�.
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

		ImGui::PopID(); // ���� ID ��
	}

	void HierarchyView::setSelectedObject(GameObject* gameObject)
	{
		// editor camera���� select�� �����Ѵ�.

		InspectorView::Inspect(gameObject);
	}

}