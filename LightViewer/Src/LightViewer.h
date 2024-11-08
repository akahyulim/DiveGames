#pragma once
#include "Dive.h"

class LightViewer
{
public:

	bool Initialize();
	bool Run();

private:
};

/*
class LightViewer : public Dive::Application
{
public:
	LightViewer();
	virtual ~LightViewer();

	void OnSetup() override;
	void OnStart() override;
	void OnStop() override;

	void HandleUpdate(const Dive::Event& e);

private:
	void initializeImGui();

private:
	Dive::Graphics* m_pGraphics;
	Dive::Input* m_pInput;

	Dive::GameObject* m_pMainCam;
	DirectX::XMFLOAT3 m_CamDefaultPos;
};
*/