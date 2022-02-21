#pragma once
#include <string>
#include "Base/Editor.h"
#include "DiveEngine.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_internal.h"

class Panel
{
public:
	Panel(Editor* pEditor, const std::string& title = "untitled");
	virtual ~Panel() = default;

	void Tick();

	float GetWidth() const	{ return m_Width; }
	float GetHeight() const	{ return m_Height; }

	bool IsVisible() const { return m_bVisible; }
	void SetVisible(bool visible) { m_bVisible = visible; }

	Dive::Scene* GetActiveScene() { return m_pActiveScene; }
	void SetActiveScene(Dive::Scene* pScene) { m_pActiveScene = pScene; }

protected:
	// �׻� �׷����� �ϴ� ��(ex. MenuBar)
	virtual void renderAlways() {}
	// â ������ Panel
	virtual void renderWindow() {}

protected:
	std::string m_Title;
	float m_Width		= 0.0f;
	float m_Height		= 0.0f;
	bool m_bVisible		= true;
	bool m_bWindow		= true;

	Editor* m_pEditor	= nullptr;

	Dive::Scene* m_pActiveScene = nullptr;

	static Dive::GameObject* m_pSelectedObject;
};