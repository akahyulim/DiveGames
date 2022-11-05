#include "HierarchyPanel.h"

namespace Editor
{
	// ���� â�� ���õ� ���¿��� esc�� ������ Selected�� �����ϰ� �ʹ�.
	// �̴� ScenePanel���� ����Ǿ�� �Ѵ�.
	HierarchyPanel::HierarchyPanel(Editor* pEditor)
		: Panel(pEditor, "Hierarchy"),
		m_pSelectedObject(nullptr)
	{
		// ���� CreateScene�� �����Ѵ�.
		// ��, ��� �г��� �����ϸ�, virtual �̺�Ʈ �ڵ鷯�� �־��� ���̴�.
	}

	HierarchyPanel::~HierarchyPanel()
	{
		DV_LOG_CLIENT_TRACE("HierarchyPanel �Ҹ��� ȣ��");
	}

	void HierarchyPanel::renderWindow()
	{
		if (!s_pActiveScene)
		{
			m_pSelectedObject = nullptr;
			return;
		}

		if (ImGui::TreeNodeEx(s_pActiveScene->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
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
					auto pID = static_cast<unsigned int*>(pPayload->Data);
					auto pDroppedObject = s_pActiveScene->GetGameObject(*pID);
					if (pDroppedObject->HasComponent<Dive::Transform>())
						pDroppedObject->GetComponent<Dive::Transform>()->SetParent(nullptr);
				}

				ImGui::EndDragDropTarget();
			}

			auto roots = s_pActiveScene->GetRoots();
			if (!roots.empty())
			{
				for (auto pRoot : roots)
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
				auto pId = static_cast<unsigned int*>(pPayload->Data);
				auto pDroppedObject = s_pActiveScene->GetGameObject(*pId);

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
			//if (m_pSelectedObject)
			//	m_pSelectedObject->Clone();
		}

		if (bRename)
		{

		}

		if (bDelete)
		{
			s_pActiveScene->RemoveGameObject(pObject);
			if (m_pSelectedObject && (m_pSelectedObject == pObject))
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
}