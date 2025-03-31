#include "View.h"

namespace Dive
{
	View::View(Editor* editor)
		: m_Title("title")
		, m_Width(0)
		, m_Height(0)
		, m_IsVisible(true)
		, m_Flags(ImGuiWindowFlags_NoCollapse)
		, m_Editor(editor)
	{
	}
	
	void View::Draw()
	{
		if (!IsVisible())
			return;

		if (ImGui::Begin(m_Title.c_str(), &m_IsVisible, m_Flags))
		{
			m_Width = ImGui::GetWindowWidth();
			m_Height = ImGui::GetWindowHeight();
		}

		drawView();

		ImGui::End();
	}
}