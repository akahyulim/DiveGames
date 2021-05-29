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
		// �̰� Menubaró�� �׻� �׷����� �κ��̴�.
		// �̿ܿ��� Progressbar, Toolbar ���� ���� ����̴�.
		TickAlways();

		if (!m_bWindow)	
			return;

		bool bBegun = false;
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
			// ���⿡�� ũ�⸦ ���ѽ�������ϴµ�...
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

			// Style�� �߰��� ������ Ǫ���ϴ� �κ��̴�.
			OnPushStyleVar();

			if (ImGui::Begin(m_title.c_str(), &m_bVisible, m_flags))
			{
				m_window = ImGui::GetCurrentWindow();
				m_height = ImGui::GetWindowHeight();

				bBegun = true;
			}
			else if (m_window && m_window->Hidden)
			{
				bBegun = true;
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

		if (bBegun)
		{
			// �̰� ������ �������� �κ��̴�.
			TickVisible();

			{
				ImGui::End();

				ImGui::PopStyleVar(m_varPushes);
				m_varPushes = 0;
			}
		}
	}
}