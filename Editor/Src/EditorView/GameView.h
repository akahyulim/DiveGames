#pragma once
#include "EditorView.h"

namespace Dive
{
	class GameView : public EditorView
	{
	public:
		GameView();
		~GameView() = default;

		void drawView() override;

	private:
	private:
		std::shared_ptr<Texture2D> m_pTex;
	};
}
