#pragma once
#include "Dive.h"

class SceneViewCamera : public Dive::Camera
{
public:
	SceneViewCamera(Dive::GameObject* pGameObject);
	~SceneViewCamera() override;

	void Update() override;

private:
	bool m_bBoost;
};
