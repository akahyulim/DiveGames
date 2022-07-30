#pragma once
#include "DvPanel.h"

namespace Editor
{
	// 다른 Panel에 접근할 수 있어야 한다.
	// SelectedObject 같은 것도 생각해두어야 한다.
	class DvMenuBarPanel : public DvPanel
	{
	public:
		DvMenuBarPanel(DvEditor* pEditor);
		~DvMenuBarPanel();

		// 위치가 조금 애매하다.
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