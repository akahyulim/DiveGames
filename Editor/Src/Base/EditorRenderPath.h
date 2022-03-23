#pragma once
#include "DiveEngine.h"

class SceneViewCamera;

class EditorRenderPath : public Dive::RenderPath
{
public:

	virtual void Update(float delta) override;
	virtual void Render() override;

	void SetActiveScene(Dive::Scene* pScene) { m_pActiveScene = pScene; }
	void SetSceneViewCamera(SceneViewCamera* pCamera) { m_pSceneViewCamera = pCamera; }

private:
private:
	Dive::Scene* m_pActiveScene = nullptr;
	SceneViewCamera* m_pSceneViewCamera = nullptr;
};