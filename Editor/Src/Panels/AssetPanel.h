#pragma once
#include "Panel.h"

namespace Editor
{
	class AssetPanel : public Panel
	{
	public:
		AssetPanel(Editor* pEditor);
		~AssetPanel();

		void renderWindow() override;

	private:
		void loadAssets();

	private:
	};
}