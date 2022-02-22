#pragma once
#include "Panel.h"

// 다른 Panel에 접근할 수 있어야 한다.
// SelectedObject 같은 것도 생각해두어야 한다.
class MenuBarPanel : public Panel
{
public:
	MenuBarPanel(Editor* pEditor);
	~MenuBarPanel();

	// 위치가 조금 애매하다.
	Dive::Scene* GetActiveScene() { return m_pActiveScene; }

	void renderAlways() override;

private:
	void menuFile();
	void menuGameObject();
	void menuComponent();
	void menuWindow();
	void menuHelp();

};