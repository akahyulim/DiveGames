#pragma once
#include "Panel.h"

class ScenePanel : public Panel
{
public:
	ScenePanel(Editor* pEditor);
	~ScenePanel() = default;

	void renderWindow() override;

private:
	Dive::Renderer* m_pRenderer = nullptr;
	//Dive::Texture2D* m_pSample = nullptr;
	ID3D11ShaderResourceView* m_pSample = nullptr;
};