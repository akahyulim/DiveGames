#pragma once
#include "Widget.h"

using namespace Dive;

// Component ������ ��/��� �մϴ�.
class Inspector : public Widget
{
public:
	Inspector(Editor* editor);
	~Inspector() = default;

	void Tick(float deltaTime = 0.0f) override;

	void SetSelected(GameObject* selected) { m_selected = selected; }

private:
	void showGameObject();
	void showTransform();

	void showAddComponentButton();

private:
	GameObject* m_selected;
};