#pragma once
#include "Widget.h"

using namespace Dive;

// DragAndDrop�� �������� ���ߴ�.
// �̴� ImGui�� �����Ͽ� �߰��ؾ� �Ѵ�.
// �ϴ� �׽�Ʈ�� ���Ǹ� ���� Serialization���� ����.
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

	// �ֽ� ���������� �ƿ� ���� ������ �����ߴ�.
	GameObject* m_clicked = nullptr;
	GameObject* m_hovered = nullptr;
};