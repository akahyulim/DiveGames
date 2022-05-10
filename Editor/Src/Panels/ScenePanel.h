#pragma once
#include "Panel.h"

class ScenePanel : public Panel
{
public:
	ScenePanel(Editor* pEditor);
	~ScenePanel();

	void OnActiveScene(const Dive::Event& e);

	void renderWindow() override;

private:
};