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
		// 매 프레임 실행은 부담될 것 같다.
		m_pActiveWorld = GEngine->GetActiveWorld();

		if (!m_pActiveWorld)
		{
			m_pSelectedObject = nullptr;
			return;
		}

		if (ImGui::TreeNodeEx(m_pActiveWorld->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
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
				// 단일 제거는 먹히지만
				// 계층구조를 지니는 대상은 에러가 발생한다.
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