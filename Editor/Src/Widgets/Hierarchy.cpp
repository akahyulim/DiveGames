#include "Hierarchy.h"


Hierarchy::Hierarchy(Editor * editor)
	: Widget(editor)
{
	m_scene = m_editor->GetContext()->GetSubsystem<Scene>().get();
	assert(m_scene);
}

void Hierarchy::Tick(float deltaTime)
{
	showTree();

	if (ImGui::IsMouseReleased(0) && m_clicked)
	{
		if (m_hovered && m_hovered->GetID() == m_clicked->GetID())
			setSelected(m_clicked);

		m_clicked = nullptr;
	}
}

void Hierarchy::showTree()
{
	m_hovered = nullptr;

	char msg[32]{ 0, };
	sprintf_s(msg, "%s - %d", m_scene->GetName().c_str(), static_cast<int>(m_scene->GetGameObjectsCount()));

	if (ImGui::TreeNodeEx(msg, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TreePop();

		auto roots = m_scene->GetRootGameObjects();
		for (auto gameObject : roots)
		{
			addGameObject(gameObject.get());
		}
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
		nodeFlags |= (m_selected->GetID() == gameObject->GetID()) ? ImGuiTreeNodeFlags_Selected : 0;

	bool openNode = ImGui::TreeNodeEx((void*)(intptr_t)gameObject->GetTypeHash(), nodeFlags, gameObject->GetName().c_str());	
	if (openNode)
	{
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
			m_hovered = gameObject;

		//handleDragAndDrop(gameObject);

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
}
