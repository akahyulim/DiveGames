#include "ScenePanel.h"
#include "imgui-docking/imgui.h"
#include "Base/Editor.h"
#include "DiveEngine.h"

ScenePanel::ScenePanel(Editor* pEditor)
	: m_pEditor(pEditor)
{
	DV_ASSERT(pEditor);
}

void ScenePanel::RenderPanel()
{
	ImGui::Begin("Scene");
	ImGui::End();
}
