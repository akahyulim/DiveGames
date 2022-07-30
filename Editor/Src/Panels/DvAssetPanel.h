#pragma once
#include "DvPanel.h"

namespace Editor
{
	class DvAssetPanel : public DvPanel
	{
	public:
		DvAssetPanel(DvEditor* pEditor);
		~DvAssetPanel();

		void renderWindow() override;

	private:
		void loadAssets();

	private:
	};
}