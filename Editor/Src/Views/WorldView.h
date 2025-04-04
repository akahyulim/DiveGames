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
		std::shared_ptr<RenderTexture> m_RenderTexture;
	};
}
