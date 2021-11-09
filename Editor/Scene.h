#pragma once
#include "Widget.h"

namespace DiveEditor
{
	class Scene : public Widget
	{
	public:
		Scene(Editor* pEditor);

		void TickVisible() override;

	private:
	private:
		DiveEngine::Scene* m_pScene;
		DiveEngine::Renderer* m_pRenderer;
	};
}