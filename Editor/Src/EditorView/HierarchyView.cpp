#include "HierarchyView.h"

namespace Dive
{
	HierarchyView::HierarchyView()
		: m_pSelectedObject(nullptr)
	{
		m_Title = "Hierarchy";
	}

	HierarchyView::~HierarchyView()
	{
	}

	void HierarchyView::drawView()
	{
		auto pActiveScene = Editor::GetInstance().GetActiveScene();
		if (!pActiveScene)
		{
			m_pSelectedObject = nullptr;
			return;
		}

		if(ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			m_pSelectedObject = nullptr;
			m_bEditName = false;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_F2) && m_pSelectedObject)
		{
			m_bEditName = true;
		}

		if (ImGui::TreeNodeEx(pActiveScene->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
				{
					auto pInstanceID = static_cast<unsigned long long*>(pPayload->Data);
					auto pDroppedObject = pActiveScene->GetGameObjectByID(*pInstanceID);
					if (pDroppedObject->HasComponent<Transform>())
						pDroppedObject->GetComponent<Transform>()->SetParent(nullptr);
				}

				ImGui::EndDragDropTarget();
			}

			auto pRoots = pActiveScene->GetRootGameObjects();
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
		if (!pObject || pObject->GetTag() == "EditorOnly")
			return;

		auto pActiveScene = Editor::GetInstance().GetActiveScene();
		if (!pActiveScene)
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

		// 각 노드 사이에 드랍 가능한 영역 추가
		ImGui::PushID(static_cast<int>(pObject->GetID())); // 고유 ID 푸시
		ImGui::Selectable("##drop_target", false, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, 2.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
			{
				auto pId = static_cast<unsigned long long*>(pPayload->Data);
				auto pDroppedObject = pActiveScene->GetGameObjectByID(*pId);

				if (pDroppedObject && pDroppedObject != pObject)
				{
					if (pDroppedObject->HasComponent<Dive::Transform>())
					{
						// 드랍된 오브젝트를 루트 노드로 설정
						pDroppedObject->GetComponent<Dive::Transform>()->SetParent(nullptr);
					}
				}
			}

			ImGui::EndDragDropTarget();
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
				auto pDroppedObject = pActiveScene->GetGameObjectByID(*pId);

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
				m_bEditName = true;
			}
		
			if (ImGui::MenuItem("Delete"))
			{
				// 현재 RemoveGameObject는 대상을 제거대상으로 마킹할 뿐이다.
				// 실제 제거는 다음 Update에서 수행한다.
				pActiveScene->RemoveGameObject(pObject);
				if (m_pSelectedObject && (m_pSelectedObject == pObject))
					m_pSelectedObject = nullptr;
			}

			ImGui::EndPopup();
		}

		if (bNodeOpen)
		{
			if (m_pSelectedObject == pObject && m_bEditName)
			{
				ImGui::SetKeyboardFocusHere();
				
				char newName[256]{};
				ImGui::InputText("##edit", newName, sizeof(newName));
				
				if (ImGui::IsKeyPressed(ImGuiKey_Enter))
				{
					pObject->SetName(newName);
					m_bEditName = false;
				}
			}
			for (auto pChild : children)
			{
				drawNode(pChild->GetGameObject());
			}

			ImGui::TreePop();
		}

		ImGui::PopID(); // 고유 ID 팝
	}
}