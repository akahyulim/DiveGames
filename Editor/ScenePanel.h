#pragma once
#include "Panel.h"

namespace Dive
{
	class ScenePanel : public Panel
	{
	public:
		ScenePanel(Editor* pEditor);

		void TickVisible() override;

	private:
	private:
		Dive::Scene* m_pScene;
		Dive::Renderer* m_pRenderer;
	};
}