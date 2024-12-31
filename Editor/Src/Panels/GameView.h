#pragma once
#include "Panel.h"

namespace Dive
{
	class GameView : public Panel
	{
	public:
		GameView(Editor* pEditor);
		~GameView() = default;

		void drawView() override;

	private:
	private:
		std::shared_ptr<Texture2D> m_pTex;
	};
}