#pragma once
#include "Panel.h"

class InspectorPanel : public Panel
{
public:
	InspectorPanel(Editor* pEditor);
	~InspectorPanel();

	void renderWindow() override;

private:
	void drawTransform(Dive::GameObject* pSelectedObject);
	void drawSpriteRenderer(Dive::GameObject* pSelectedObject);
	void drawMeshRenderer(Dive::GameObject* pSelectedObject);
	
private:
};