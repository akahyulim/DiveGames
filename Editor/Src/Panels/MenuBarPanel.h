#pragma once
#include "Panel.h"
#include "Importer/ModelImporter.h"

namespace Editor
{
	// �ٸ� Panel�� ������ �� �־�� �Ѵ�.
	// SelectedObject ���� �͵� �����صξ�� �Ѵ�.
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