#pragma once
#include "Widget.h"
#include "../DragAndDrop.h"

using namespace Dive;


// Scene에 구성된 GameObject들을 계층 구조 형태로 관리합니다.
class Hierarchy : public Widget
{
public:
	Hierarchy(Editor* editor);

	void Tick(float deltaTime = 0.0f) override;

private:
	void showTree();
	void addGameObject(GameObject* gameObject);

	void setSelected(GameObject* gameObject);
	void handleClicking();
	void handleDragAndDrop(GameObject* gameObject);

	void popupGameObjectPropertyMenu();
	void popupGameObjectRename();

private:
	Scene* m_scene = nullptr;

	GameObject* m_selected = nullptr;
	GameObject* m_clicked = nullptr;
	GameObject* m_hovered = nullptr;
	GameObject* m_copied = nullptr;

	DragDropPayload m_payload;
	bool m_bPopupRename;
};