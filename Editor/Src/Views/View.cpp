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
	
	void View::ComposeUI()
	{
		if (!IsVisible())
			return;

		if (ImGui::Begin(m_Title.c_str(), &m_IsVisible, m_Flags))
		{
			m_Width = ImGui::GetWindowWidth();
			m_Height = ImGui::GetWindowHeight();
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
