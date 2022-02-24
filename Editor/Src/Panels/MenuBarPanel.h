#pragma once
#include "Panel.h"

class EditorScene;

// �ٸ� Panel�� ������ �� �־�� �Ѵ�.
// SelectedObject ���� �͵� �����صξ�� �Ѵ�.
class MenuBarPanel : public Panel
{
public:
	MenuBarPanel(Editor* pEditor);
	~MenuBarPanel();

	// ��ġ�� ���� �ָ��ϴ�.
	EditorScene* GetActiveScene() { return m_pActiveScene; }

	void renderAlways() override;

private:
	void menuFile();
	void menuGameObject();
	void menuComponent();
	void menuWindow();
	void menuHelp();

};