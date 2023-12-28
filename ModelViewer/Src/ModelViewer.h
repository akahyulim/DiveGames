#pragma once
#include "Dive.h"
#include "ModelImporter.h"

class Panel;

class ModelViewer : public Dive::Application
{
public:
	ModelViewer();
	~ModelViewer() = default;

	void Setup() override;
	void Start() override;
	void Stop() override;

	void OnPostRender(const Dive::Event& e);
	void OnWindowEvent(const Dive::Event& e);

	Dive::GameObject* GetCamera() { return m_pCamera; }

private:
	ModelImporter m_ModelImporter;

	Dive::GameObject* m_pCamera;
	Dive::GameObject* m_pLoadedModel;
	Dive::GameObject* m_pSelectedNode;

	std::vector<std::unique_ptr<Panel>> m_Panels;
};