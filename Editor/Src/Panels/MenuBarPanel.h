#pragma once

class Editor;

// Window Menu를 넣을거면 m_bShow 같은 걸로 제어 가능하다.
// 문제는 다른 Panel에 접근할 수 있어야 한다는 것...
// SelectedObject 같은 것도 생각해두어야 한다.
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