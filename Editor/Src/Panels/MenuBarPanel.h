#pragma once

class Editor;

// Window Menu�� �����Ÿ� m_bShow ���� �ɷ� ���� �����ϴ�.
// ������ �ٸ� Panel�� ������ �� �־�� �Ѵٴ� ��...
// SelectedObject ���� �͵� �����صξ�� �Ѵ�.
class MenuBarPanel
{
public:
	MenuBarPanel(Editor* pEditor);
	~MenuBarPanel() = default;

	void RenderPanel();

private:
	void menuFile();
	void menuGameObject();
	void menuComponent();
	void menuHelp();

private:
	Editor* m_pEditor = nullptr;
};