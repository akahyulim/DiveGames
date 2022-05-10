#include "HierarchyPanel.h"

HierarchyPanel::HierarchyPanel(Editor* pEditor)
	: Panel(pEditor, "Hierarchy")
{
	SUBSCRIBE_EVENT(eEventType::CreateScene, EVENT_HANDLER(OnActiveScene));
}

HierarchyPanel::~HierarchyPanel()
{
}

void HierarchyPanel::OnActiveScene(const Dive::Event& e)
{
	const auto& evt = dynamic_cast<const Dive::CreateSceneEvent&>(e);
	m_pActiveScene = const_cast<Dive::Scene*>(evt.GetCreatedScene());
}

void HierarchyPanel::renderWindow()
{
	if (!m_pActiveScene)
	{
		m_pSelectedObject = nullptr;
		return;
	}

	if (ImGui::TreeNodeEx(m_pActiveScene->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
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
				auto pDroppedObject = m_pActiveScene->GetGameObject(*pInstanceID);
				if(pDroppedObject->HasComponent<Dive::Transform>())
					pDroppedObject->GetComponent<Dive::Transform>()->SetParent(nullptr);
			}

			ImGui::EndDragDropTarget();
		}

		auto pRoots = m_pActiveScene->GetRoots();
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
		nodeFlags |= (m_pSelectedObject->GetInstanceID() == pObject->GetInstanceID()) ? ImGuiTreeNodeFlags_Selected : 0;
	}

	bool bNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)pObject->GetInstanceID(), nodeFlags, pObject->GetName().c_str());

	if (ImGui::IsItemClicked())
	{
		m_pSelectedObject = pObject;
	}

	if (ImGui::BeginDragDropSource())
	{
		unsigned long long id = pObject->GetInstanceID();
		ImGui::SetDragDropPayload("HIERARCHY_NODE", &id, sizeof(unsigned long long));
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
		{
			auto pId = static_cast<unsigned long long*>(pPayload->Data);
			auto pDroppedObject = m_pActiveScene->GetGameObject(*pId);

			if (pDroppedObject->GetInstanceID() != pObject->GetInstanceID())
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

	}

	if (bRename)
	{

	}

	if (bDelete)
	{
		m_pActiveScene->RemoveGameObject(pObject);
		if (m_pSelectedObject && (m_pSelectedObject->GetInstanceID() == pObject->GetInstanceID()))
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
