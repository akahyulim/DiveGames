#include "HierarchyPanel.h"

namespace Editor
{
	// 현재 창이 선택된 상태에서 esc를 누르면 Selected를 해제하고 싶다.
	// 이는 ScenePanel에도 적용되어야 한다.
	HierarchyPanel::HierarchyPanel(Editor* pEditor)
		: Panel(pEditor, "Hierarchy"),
		m_pSelectedObject(nullptr)
	{
		// 역시 CreateScene를 구독한다.
		// 즉, 모든 패널이 구독하며, virtual 이벤트 핸들러가 있었을 것이다.
	}

	HierarchyPanel::~HierarchyPanel()
	{
		DV_LOG_CLIENT_TRACE("HierarchyPanel 소멸자 호출");
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