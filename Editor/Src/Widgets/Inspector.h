#pragma once
#include "Widget.h"

using namespace Dive;

// Component 정보를 입/출력 합니다.
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