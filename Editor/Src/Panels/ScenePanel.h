#pragma once

class Editor;

class ScenePanel
{
public:
	ScenePanel(Editor* pEditor);
	~ScenePanel() = default;

	void RenderPanel();

private:

private:
	Editor* m_pEditor = nullptr;
};