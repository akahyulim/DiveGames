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
	
private:
};