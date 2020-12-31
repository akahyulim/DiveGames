#include "Hierarchy.h"


Hierarchy::Hierarchy(Editor * editor)
	: Widget(editor)
{
	m_title = std::move("Hierarchy");
	m_scene = m_editor->GetContext()->GetSubsystem<Scene>().get();
	assert(m_scene);
}

void Hierarchy::Tick(float deltaTime)
{
	showTree();

	if (ImGui::IsMouseReleased(0) && m_clicked)
	{
		if (m_hovered && m_hovered->GetID() == m_clicked->GetID())
		{
			setSelected(m_clicked);
		}

		m_clicked = nullptr;
	}
}

void Hierarchy::showTree()
{
	m_hovered = nullptr;

	char msg[32]{ 0, };
	sprintf_s(msg, "%s - %d", m_scene->GetName().c_str(), m_scene->GetGameObjectsCount());

	if (ImGui::TreeNodeEx(msg, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (auto payload = DragDrop::Get().GetPayload(eDragPayloadType::Actor))
		{
			auto id = std::get<unsigned int>(payload->data);
			if (auto droppedObj = m_scene->GetGameObjectByID(id))
			{
				droppedObj->GetTransform()->SetParent(nullptr);
			}
		}

		auto roots = m_scene->GetRootGameObjects();
		for (const auto& gameObject : roots)
		{
			addGameObject(gameObject.get());
		}

		ImGui::TreePop();
	}

	handleClicking();
}

void Hierarchy::addGameObject(GameObject * gameObject)
{
	if (!gameObject)
		return;

	// set node flags
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;
	
	auto children = gameObject->GetComponent<Transform>()->GetChildren();
	nodeFlags |= children.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow;

	if (m_selected)
	{
		nodeFlags |= (m_selected->GetID() == gameObject->GetID()) ? ImGuiTreeNodeFlags_Selected : 0;
	}

	bool openNode = ImGui::TreeNodeEx((void*)(intptr_t)gameObject->GetID(), nodeFlags, gameObject->GetName().c_str());	
	
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
	{
		m_hovered = gameObject;
	}

	handleDragAndDrop(gameObject);

	if (openNode)
	{
		for (auto child : children)
		{
			addGameObject(child->GetOwner());
		}

		ImGui::TreePop();
	}
}

void Hierarchy::setSelected(GameObject * gameObject)
{
	if (!gameObject)
		return;

	m_selected = gameObject;
	// widget Inspector
	// widget menu에도 전달
}

void Hierarchy::handleClicking()
{
	if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		return;

	bool clickedLeft = ImGui::IsMouseClicked(0);
	bool clickedRight = ImGui::IsMouseClicked(1);

	if (clickedLeft)
	{
		if (m_hovered)
			m_clicked = m_hovered;
		else
			setSelected(nullptr);
	}
}

void Hierarchy::handleDragAndDrop(GameObject * gameObject)
{
	// 드래그일 경우 현재 오브젝트의 정보를 payload에 저장한다.
	if (DragDrop::Get().DragBegin())
	{
		// 멤버 변수일 필요가...?
		m_payload.data = gameObject->GetID();
		m_payload.type = eDragPayloadType::Actor;
		DragDrop::Get().DragPayload(m_payload);
		DragDrop::Get().DragEnd();
	}

	// 드랍일 경우 적재 오브젝트를 확인하고, 현재 오브젝트와 다를 경우 현재 오브젝트를 부모로 설정한다.
	if (auto payload = DragDrop::Get().GetPayload(eDragPayloadType::Actor))
	{
		auto id = std::get<unsigned int>(payload->data);
		if (auto droppedObj = m_scene->GetGameObjectByID(id))
		{
			if (droppedObj->GetID() != gameObject->GetID())
			{
				droppedObj->GetTransform()->SetParent(gameObject->GetTransform());
			}
		}
	}
}
