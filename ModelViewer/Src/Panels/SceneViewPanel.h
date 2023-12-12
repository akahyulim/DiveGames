#pragma once
#include "Panel.h"

class SceneViewPanel : public Panel
{
public:
	SceneViewPanel();
	~SceneViewPanel() override;

	void OnRender() override;

private:
};