#include "DvPanel.h"

namespace Editor
{
	DvPanel::DvPanel(DvEditor* pEditor, const std::string& title)
		: m_pEditor(pEditor), m_Title(title)
	{
		DV_ASSERT(pEditor != nullptr);
	}

	void DvPanel::Tick()
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