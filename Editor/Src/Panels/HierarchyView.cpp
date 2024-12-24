#include "HierarchyView.h"

namespace Dive
{
	HierarchyView::HierarchyView(Editor* pEditor)
		: Panel(pEditor)
		, m_pSelectedObject(nullptr)
	{
		m_Title = "Hierarchy";
	}

	HierarchyView::~HierarchyView()
	{
	}

	void HierarchyView::renderView()
	{
		// �� ������ ������ �δ�� �� ����.
		m_pActiveWorld = GEngine->GetActiveWorld();

		if (!m_pActiveWorld)
		{
			m_pSelectedObject = nullptr;
			return;
		}

		if (ImGui::TreeNodeEx(m_pActiveWorld->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			// �ϴ� �� ���̶� ���� �Ʒ� �κп���
			// Transform ���� ������ �߻��Ѵ�.
			// ���� Transform�� ���� GameObject�� ������� �Ͽ����Ƿ�
			// �̴� �ݵ�� �����Ǿ�� �Ѵ�.
			// => ���������� ������ ���װ� �߻��Ѵ�. �ٸ� ������ SelectedObject�� Transform�� �䱸�ϴ� �� ����.
			// => ScenePanel���� ImGuizmo ������ �߻��� ��������. �ϴ� ���� �� �׽�Ʈ�� �Ϸ��ߴ�.
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
				{
					auto pInstanceID = static_cast<unsigned long long*>(pPayload->Data);
					auto pDroppedObject = m_pActiveWorld->GetGameObjectByID(*pInstanceID);
					if (pDroppedObject->HasComponent<Transform>())
						pDroppedObject->GetComponent<Transform>()->SetParent(nullptr);
				}

				ImGui::EndDragDropTarget();
			}

			auto pRoots = m_pActiveWorld->GetRootGameObjects();
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

	void HierarchyView::drawNode(GameObject* pObject)
	{
		if (!pObject)
			return;

		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;

		std::vector<Dive::Transform*> children;
		if (pObject->HasComponent<Dive::Transform>())
		{
			children = pObject->GetComponent<Dive::Transform>()->GetChildren();
		}

		children.empty() ? nodeFlags |= ImGuiTreeNodeFlags_Leaf : nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
		if (m_pSelectedObject)
		{
			nodeFlags |= (m_pSelectedObject == pObject) ? ImGuiTreeNodeFlags_Selected : 0;
		}

		bool bNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)pObject->GetID(), nodeFlags, pObject->GetName().c_str());

		if (ImGui::IsItemClicked())
		{
			m_pSelectedObject = pObject;
		}

		if (ImGui::BeginDragDropSource())
		{
			auto id = pObject->GetID();
			ImGui::SetDragDropPayload("HIERARCHY_NODE", &id, sizeof(unsigned long long));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
			{
				auto pId = static_cast<unsigned long long*>(pPayload->Data);
				auto pDroppedObject = m_pActiveWorld->GetGameObjectByID(*pId);

				if (pDroppedObject != pObject)
				{
					if (pDroppedObject->HasComponent<Dive::Transform>())
					{
						pDroppedObject->GetComponent<Dive::Transform>()->SetParent(
							pObject->GetComponent<Dive::Transform>());
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
			}
		
			if (ImGui::MenuItem("Delete"))
			{
				// ���� ���Ŵ� ��������
				// ���������� ���ϴ� ����� ������ �߻��Ѵ�.
				m_pActiveWorld->RemoveGameObject(pObject);
				if (m_pSelectedObject && (m_pSelectedObject == pObject))
					m_pSelectedObject = nullptr;
			}

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
	}
}