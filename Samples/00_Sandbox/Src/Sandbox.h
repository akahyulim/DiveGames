#pragma once
#include "DiveEngine.h"

class Sandbox : public Dive::Application
{
public:
	explicit Sandbox(Dive::Context* pContext);
	~Sandbox() override;

	void Setup() override;
	void Start() override;
	void Stop() override;

	void OnUpdate(const Dive::Event& evnt);

private:
	void createScene();
	Dive::Model* getModel(const std::string& name);
	Dive::Technique* getTechnique(const std::string& name);

	void moveCamera(float delta);

private:
	Dive::Scene* m_pScene;
	Dive::GameObject* m_pMainCamera;
};
