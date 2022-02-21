#include "Panel.h"

Dive::GameObject* Panel::m_pSelectedObject = nullptr;

Panel::Panel(Editor* pEditor, const std::string& title)
	: m_pEditor(pEditor), m_Title(title)
{
	DV_ASSERT(pEditor != nullptr);
}

void Panel::Tick()
{
	if (!IsVisible())
		return;

	renderAlways();

	if (!m_bWindow)
		return;

	// begin
	ImGui::Begin(m_Title.c_str());

	m_Width = ImGui::GetWindowWidth();
	m_Height = ImGui::GetWindowHeight();

	renderWindow();

	// end
	ImGui::End();
}
