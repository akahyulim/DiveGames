#pragma once
#include "DiveEngine.h"

//======================================================================//
//	Forward Lighting + Font + FPS, CPU Usage, Timers + Frustum Culling	//
//======================================================================//	
class ForwardLights : public Dive::Application
{
public:
	explicit ForwardLights(Dive::Context* pContext);
	~ForwardLights() override;

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