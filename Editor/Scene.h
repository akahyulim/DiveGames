#pragma once
#include "Widget.h"

namespace Editor
{
	class Scene : public Widget
	{
	public:
		Scene(Editor* pEditor);

		void TickVisible() override;

	private:
	private:
		dive::Scene* m_pScene;
		dive::Renderer* m_pRenerer;

		// test 
		dive::dvTexture2D* m_pChoA = nullptr;
	};
}