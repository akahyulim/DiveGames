#pragma once
#include "Widget.h"

using namespace Dive;

class EditView : public Widget
{
public:
	EditView(Editor* editor);
	~EditView();

	void Tick(float deltaTime = 0.0f) override;

private:
	void drawTexture(float deltaTime = 0.0f);

private:
	Graphics* m_graphics;
	Dive_Texture* m_texture;
};
