#include "Panel.h"
#include "../Editor.h"

namespace Dive
{
	Panel::Panel(Editor* pEditor)
		: m_pEditor(pEditor)
		, m_Title("title")
		, m_Width(0)
		, m_Height(0)
		, m_bVisible(true)
	{
	}
	
	void Panel::Draw()
	{
		if (!IsVisible())
			return;

		ImGui::Begin(m_Title.c_str());

		m_Width = ImGui::GetWindowWidth();
		m_Height = ImGui::GetWindowHeight();

		drawView();

		ImGui::End();
	}
}