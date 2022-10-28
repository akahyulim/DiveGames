#pragma once
#include "Panel.h"

namespace Editor
{
	class ScenePanel : public Panel
	{
	public:
		ScenePanel(Editor* pEditor);
		~ScenePanel() override;

		void renderWindow() override;

	private:
		std::unique_ptr<Dive::Texture2D> m_pSceneRenderTarget;
		Dive::Viewport* m_pSceneViewport = nullptr;
	};
}