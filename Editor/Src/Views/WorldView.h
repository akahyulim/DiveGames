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
		RenderTexture* m_RenderTarget = nullptr;
	};
}
