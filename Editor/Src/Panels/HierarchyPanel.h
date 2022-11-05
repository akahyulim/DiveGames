#pragma once
#include "Panel.h"

namespace Editor
{
	class HierarchyPanel : public Panel
	{
	public:
		HierarchyPanel(Editor* pEditor);
		~HierarchyPanel();

		void renderWindow() override;


	private:
		void drawNode(Dive::GameObject* pObject);
		void setSelectedObject(Dive::GameObject* pObject);

	private:
		// 이것두 static으로 만들면 되지 않을까?
		Dive::GameObject* m_pSelectedObject;
	};
}