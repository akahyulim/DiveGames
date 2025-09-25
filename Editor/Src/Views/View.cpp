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

	DirectX::XMUINT2 View::GetMousePosition() const
	{
		ImVec2 screenMousePos = ImGui::GetMousePos();
		ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
		ImVec2 windowPos = ImGui::GetWindowPos();

		ImVec2 contentPos = { windowPos.x + contentMin.x, windowPos.y + contentMin.y };
		ImVec2 localMousePos = { screenMousePos.x - contentPos.x, screenMousePos.y - contentPos.y };

		return { (uint32_t)localMousePos.x, (uint32_t)localMousePos.y };
	}
}
