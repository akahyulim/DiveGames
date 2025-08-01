#pragma once
#include "View.h"

namespace Dive
{
	class WorldView : public View
	{
	public:
		WorldView(Editor* editor);
		~WorldView();

		void drawView() override;

	private:
		std::unique_ptr<RenderTexture> m_renderTarget;
	};
}
