#pragma once
#include "Panel.h"

namespace Editor
{
	class InspectorPanel : public Panel
	{
	public:
		InspectorPanel(Editor* pEditor);
		~InspectorPanel();

		void SetSelectedObject(Dive::GameObject* pObject);

		void renderWindow() override;

	private:
		//void drawTransform(Dive::GameObject* pSelectedObject);
		//void drawSpriteRenderer(Dive::GameObject* pSelectedObject);
		//void drawMeshRenderer(Dive::GameObject* pSelectedObject);
		//void drawLight(Dive::GameObject* pSelectedObject);


	private:
	};
}