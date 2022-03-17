#pragma once
#include "DiveEngine.h"

class SceneViewCamera;

class EditorScene : public Dive::Scene
{
public:
	EditorScene(const std::string name = std::string());
	~EditorScene() = default;

	void UpdateEditor(float elapsedTime, SceneViewCamera* pCamera);

private:

};
