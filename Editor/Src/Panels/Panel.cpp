#include "Panel.h"

namespace Editor
{
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
		// 이 곳에서 flags를 추가해야 스크롤바가 안 생긴다.
		ImGui::Begin(m_Title.c_str());

		m_Width = ImGui::GetWindowWidth();
		m_Height = ImGui::GetWindowHeight();

		renderWindow();

		// end
		ImGui::End();
	}

}