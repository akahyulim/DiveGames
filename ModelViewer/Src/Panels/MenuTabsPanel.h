#pragma once
#include "Panel.h"
#include "Dive.h"
#include "../ModelImporter.h"

class MenuTabsPanel : public Panel
{
public:
	MenuTabsPanel();
	~MenuTabsPanel() override;

	void Initialize(Dive::GameObject* pCameraObject);

	void OnRender() override;

private:
	void showFileTab();
	void showCameraTab();
	void showEnvironmentTab();
	void showModelTab();

private:
	ModelImporter m_ModelImporter;	// ���� static �Լ� Ŭ������ ����
	
	Dive::GameObject* m_pCameraObject;
	Dive::GameObject* m_pLoadedModel;
};