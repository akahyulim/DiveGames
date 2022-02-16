#pragma once
#include "Panel.h"

class ScenePanel : public Panel
{
public:
	ScenePanel(Editor* pEditor);
	~ScenePanel() = default;

	void renderWindow() override;

private:
};