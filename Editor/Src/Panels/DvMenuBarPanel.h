#pragma once
#include "DvPanel.h"

namespace Editor
{
	// �ٸ� Panel�� ������ �� �־�� �Ѵ�.
	// SelectedObject ���� �͵� �����صξ�� �Ѵ�.
	class DvMenuBarPanel : public DvPanel
	{
	public:
		DvMenuBarPanel(DvEditor* pEditor);
		~DvMenuBarPanel();

		// ��ġ�� ���� �ָ��ϴ�.
		Dive::Scene* GetActiveScene() {
			return nullptr;// m_pActiveScene;
		}

		void OnActiveScene(const Dive::Event& e);

		void renderAlways() override;

	private:
		void menuFile();
		void menuGameObject();
		void menuComponent();
		void menuTools();
		void menuWindow();
		void menuHelp();

	private:

	};
}