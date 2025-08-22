#include "View.h"

namespace Dive
{
	View::View(Editor* editor)
		: m_title("title")
		, m_width(0)
		, m_height(0)
		, m_isVisible(true)
		, m_flags(ImGuiWindowFlags_NoCollapse)
		, m_editor(editor)
	{
	}
	
	void View::ComposeUI()
	{
		if (!IsVisible())
			return;

		if (ImGui::Begin(m_title.c_str(), &m_isVisible, m_flags))
		{
			m_width = ImGui::GetWindowWidth();
			m_height = ImGui::GetWindowHeight();
		}

		renderContent();

		ImGui::End();
	}
}
