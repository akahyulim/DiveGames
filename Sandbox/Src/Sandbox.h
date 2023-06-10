#pragma once
#include "Dive.h"

class Sandbox : public Dive::Application
{
public:
	Sandbox();
	~Sandbox() override;

	void Setup() override;
	void Start() override;
	void Stop() override;

	void OnUpdate(const Dive::Event& evnt);

private:

private:
	Dive::Camera* m_pMainCamera;

	Dive::Model* m_pPilot;
	Dive::Model* m_pStormTrooper;
};