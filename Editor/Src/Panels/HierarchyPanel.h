#pragma once
#include "Panel.h"

class HierarchyPanel : public Panel
{
public:
	HierarchyPanel(Editor* pEditor);
	~HierarchyPanel();

	void renderWindow() override;

private:
private:
};