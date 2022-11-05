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
		// �̰͵� static���� ����� ���� ������?
		Dive::GameObject* m_pSelectedObject;
	};
}