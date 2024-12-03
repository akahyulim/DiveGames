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
		, m_bWindow(true)
	{
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