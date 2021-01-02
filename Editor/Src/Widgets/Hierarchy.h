#pragma once
#include "Widget.h"
#include "../DragAndDrop.h"

using namespace Dive;


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

	// 최신 버전에서는 아에 전역 변수로 선언했다.
	GameObject* m_clicked = nullptr;
	GameObject* m_hovered = nullptr;

	DragDropPayload m_payload;
};