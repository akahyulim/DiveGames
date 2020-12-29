#pragma once
#include "Widget.h"


class Hierarchy : public Widget
{
public:
	Hierarchy(Editor* editor);

	void Tick(float deltaTime = 0.0f) override;

private:
	void showTree();

private:
	Scene* m_scene;
};