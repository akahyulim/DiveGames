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
		// �� ������ flags�� �߰��ؾ� ��ũ�ѹٰ� �� �����.
		ImGui::Begin(m_Title.c_str());

		m_Width = ImGui::GetWindowWidth();
		m_Height = ImGui::GetWindowHeight();

		renderWindow();

		// end
		ImGui::End();
	}
}