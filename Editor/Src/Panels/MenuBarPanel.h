#pragma once
#include "Panel.h"

namespace Editor
{
	class ModelImporter;

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

		void modelImporterWindow();

	private:
		ModelImporter* m_pModelImporter;
	};
}