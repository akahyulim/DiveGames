#pragma once
#include "IWidget.h"

namespace Dive
{
	class Renderer;
	class Texture2D;
}

class Editor;

class Scene : public IWidget
{
public:
	Scene(Editor* editor);
	~Scene() = default;

	void Tick(float deltaTime = 0.0f) override;

private:
private:
	std::shared_ptr<Dive::Renderer> m_Renderer;
	std::shared_ptr<Dive::Texture2D> m_RenderTex;
};

