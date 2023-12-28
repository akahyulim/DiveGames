#pragma once
#include "Panel.h"

class SceneViewPanel : public Panel
{
public:
	SceneViewPanel(ModelViewer* pModelViewer);
	~SceneViewPanel() = default;

	void OnRender() override;

private:
};