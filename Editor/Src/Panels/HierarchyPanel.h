#pragma once
#include "Panel.h"

class HierarchyPanel : public Panel
{
public:
	HierarchyPanel(Editor* pEditor);
	~HierarchyPanel();

	void OnActiveScene(const Dive::Event& e);

	void renderWindow() override;

	Dive::GameObject* GetSeletecedObject() { return m_pSelectedObject; }

private:
	void drawNode(Dive::GameObject* pObject);

private:
	Dive::GameObject* m_pSelectedObject = nullptr;
};