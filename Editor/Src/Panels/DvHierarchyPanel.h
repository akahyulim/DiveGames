#pragma once
#include "DvPanel.h"

namespace Editor
{
	class DvHierarchyPanel : public DvPanel
	{
	public:
		DvHierarchyPanel(DvEditor* pEditor);
		~DvHierarchyPanel();

		void OnActiveScene(const Dive::Event& e);

		void renderWindow() override;

		//Dive::GameObject* GetSeletecedObject() { return m_pSelectedObject; }

	private:
		//void drawNode(Dive::GameObject* pObject);

	private:
		Dive::GameObject* m_pSelectedObject = nullptr;
	};
}