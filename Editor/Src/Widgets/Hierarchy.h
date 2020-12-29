#pragma once
#include "Widget.h"

using namespace Dive;

// DragAndDrop을 구현하지 못했다.
// 이는 ImGui를 랩핑하여 추가해야 한다.
// 일단 테스트의 편의를 위해 Serialization부터 간다.
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

private:
	Scene* m_scene = nullptr;

	GameObject* m_selected = nullptr;

	// 최신 버전에서는 아에 전역 변수로 선언했다.
	GameObject* m_clicked = nullptr;
	GameObject* m_hovered = nullptr;
};