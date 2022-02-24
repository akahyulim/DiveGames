#pragma once
#include "DiveEngine.h"

class EditorCamera;

class EditorScene : public Dive::Scene
{
public:
	EditorScene(const std::string name = std::string());
	~EditorScene() = default;

	void UpdateEditor(float elapsedTime, EditorCamera* pCamera);

private:

};
