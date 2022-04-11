#pragma once
#include "Panel.h"

class ScenePanel : public Panel
{
public:
	ScenePanel(Editor* pEditor);
	~ScenePanel();

	void renderWindow() override;

private:
};