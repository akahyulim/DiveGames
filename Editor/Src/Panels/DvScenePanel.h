#pragma once
#include "DvPanel.h"

namespace Editor
{
	class DvScenePanel : public DvPanel
	{
	public:
		DvScenePanel(DvEditor* pEditor);
		~DvScenePanel();

		//void OnActiveScene(const Dive::Event& e);

		void renderWindow() override;

	private:
	};
}