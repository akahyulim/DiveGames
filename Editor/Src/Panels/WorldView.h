#pragma once
#include "Panel.h"

namespace Dive
{
	class WorldView : public Panel
	{
	public:
		WorldView(Editor* pEditor);
		~WorldView() = default;

		void renderView() override;

	private:
	private:
		std::shared_ptr<Texture2D> m_pTex;
	};
}