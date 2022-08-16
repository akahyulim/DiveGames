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

	private:
		// �̰͵� static���� ����� ���� ������?
		Dive::GameObject* m_pSelectedObject;
	};
}