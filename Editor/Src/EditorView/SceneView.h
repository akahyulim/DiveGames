#pragma once
#include "EditorView.h"

namespace Dive
{
	class SceneView : public EditorView
	{
	public:
		SceneView();
		~SceneView();

		void drawView() override;

		Texture2D* GetRenderTarget() const { return m_pRenderTarget; }

	private:
		Texture2D* m_pRenderTarget;
	};
}