#pragma once
#include "Panel.h"
#include "Importer/ModelImporter.h"

namespace Editor
{
	// 다른 Panel에 접근할 수 있어야 한다.
	// SelectedObject 같은 것도 생각해두어야 한다.
	class MenuBarPanel : public Panel
	{
	public:
		MenuBarPanel(Editor* pEditor);
		~MenuBarPanel();

		void renderAlways() override;

	private:
		void menuFile();
		void menuGameObject();
		void menuComponent();
		void menuTools();
		void menuWindow();
		void menuHelp();

	private:
		ModelImporter m_ModelImporter;
	};
}