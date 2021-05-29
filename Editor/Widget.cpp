#include "Widget.h"
#include "Editor.h"

namespace Editor
{
	Widget::Widget(Editor* pEditor)
	{
		assert(pEditor);

		m_pEditor = pEditor;
	}

	void Widget::Tick()
	{
		// �̰� �̸�ó�� �׻� �׷����� �κ��̰�
		TickAlways();

		if (!m_bWindow)	
			return;

		// �Ʒ��� �κ��� Ư�� ��Ȳ??? ������ �������� �� ����.
		// MenuBar�δ� ������ �Ұ����ϴ�.
		bool begun = false;
		{
			if (!m_bVisible)
				return;

			if (m_position.x != -1.0f && m_position.y != -1.0f)
			{
				ImGui::SetNextWindowPos(m_position);
			}
			if (m_size.x != -1.0f && m_size.y != -1.0f)
			{
				ImGui::SetNextWindowSize(m_size, ImGuiCond_FirstUseEver);
			}
			if ((m_size.x != -1.0f && m_size.y != -1.0f) || (m_sizeMax.x != FLT_MAX && m_sizeMax.y != FLT_MAX))
			{
				ImGui::SetNextWindowSizeConstraints(m_size, m_sizeMax);
			}
			if (m_alpha != -1.0f)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_alpha);
				m_varPushes++;
			}
			if (m_padding.x != -1.0f && m_padding.y != -1.0f)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_padding);
				m_varPushes++;
			}

			OnPushStyleVar();

			// ������ ��ü ������ ���� �׷��� �� ȣ��ȴ�.
			// ���� �ᱹ �ٸ� �����츦 ����� �׸��� ���°� �Ǿ���...
			if (ImGui::Begin(m_title.c_str(), &m_bVisible, m_flags))
			{
				m_window = ImGui::GetCurrentWindow();
				m_height = ImGui::GetWindowHeight();

				begun = true;
			}
			else if (m_window && m_window->Hidden)
			{
				begun = true;
			}

			if (m_window && m_window->Appearing)
			{
				OnShow();
			}
			else if (!m_bVisible)
			{
				OnHide();
			}
		}

		if (begun)
		{
			TickVisible();

			{
				ImGui::End();

				ImGui::PopStyleVar(m_varPushes);
				m_varPushes = 0;
			}
		}
	}
}