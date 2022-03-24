#pragma once
#include "Panel.h"

// �ٸ� Panel�� ������ �� �־�� �Ѵ�.
// SelectedObject ���� �͵� �����صξ�� �Ѵ�.
class MenuBarPanel : public Panel
{
public:
	MenuBarPanel(Editor* pEditor);
	~MenuBarPanel();

	// ��ġ�� ���� �ָ��ϴ�.
	Dive::Scene* GetActiveScene() { return m_pActiveScene; }

	void renderAlways() override;

private:
	void menuFile();
	void menuGameObject();
	void menuComponent();
	void menuWindow();
	void menuHelp();

private:
	// test asset
	Dive::Texture2D* m_pIU = nullptr;
	Dive::Texture2D* m_pChoA = nullptr;

};