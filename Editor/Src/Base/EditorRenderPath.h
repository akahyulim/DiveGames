#pragma once
#include "DiveEngine.h"

class SceneViewCamera;

class EditorRenderPath : public Dive::RenderPath
{
public:

	virtual void Update(float delta) override;
	virtual void Render() override;

	void SetSceneViewCamera(SceneViewCamera* pCamera) { m_pSceneViewCamera = pCamera; }

private:
private:
	SceneViewCamera* m_pSceneViewCamera = nullptr;
};