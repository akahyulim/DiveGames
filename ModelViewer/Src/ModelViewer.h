#pragma once
#include "Dive.h"
#include "ModelImporter.h"
#include "Panels/SceneViewPanel.h"
#include "Panels/MenuTabsPanel.h"

class ModelViewer : public Dive::Application
{
public:
	ModelViewer();
	~ModelViewer() override = default;

	void Setup() override;
	void Start() override;
	void Stop() override;

	void OnPostRender(const Dive::Event& e);
	void OnWindowEvent(const Dive::Event& e);

private:
	ModelImporter m_ModelImporter;

	Dive::GameObject* m_pCamera;
	Dive::GameObject* m_pLoadedModel;
	Dive::GameObject* m_pSelectedNode;

	SceneViewPanel m_SceneViewPanel;
	MenuTabsPanel m_MenuTabsPanel;
};