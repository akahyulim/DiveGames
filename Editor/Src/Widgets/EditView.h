#pragma once
#include "Widget.h"

namespace Dive { class EditorRenderer; }

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
	EditorRenderer* m_renderer;
	
	Dive_Texture* m_texture;
};
