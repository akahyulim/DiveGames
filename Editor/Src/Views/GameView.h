#pragma once
#include "View.h"

namespace Dive
{
	class GameView : public View
	{
	public:
		GameView(Editor* editor);
		~GameView();

		void drawView() override;

	private:
		std::unique_ptr<RenderTexture> m_renderTarget;
	};
}
