#pragma once
#include "Panel.h"

class HierarchyPanel : public Panel
{
public:
	HierarchyPanel(Editor* pEditor);
	~HierarchyPanel();

	void renderWindow() override;

private:
	void drawNode(Dive::GameObject* pObject);

private:
	Dive::GameObject* m_pSelectedObject = nullptr;
};