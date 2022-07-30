#pragma once
#include "DvPanel.h"

namespace Editor
{
	class DvInspectorPanel : public DvPanel
	{
	public:
		DvInspectorPanel(DvEditor* pEditor);
		~DvInspectorPanel();

		void OnActiveScene(const Dive::Event& e);

		void renderWindow() override;

	private:
		//void drawTransform(Dive::GameObject* pSelectedObject);
		//void drawSpriteRenderer(Dive::GameObject* pSelectedObject);
		//void drawMeshRenderer(Dive::GameObject* pSelectedObject);
		//void drawLight(Dive::GameObject* pSelectedObject);

	private:
	};
}