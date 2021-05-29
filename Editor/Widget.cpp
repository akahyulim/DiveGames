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
		// 이건 Menubar처럼 항상 그려지는 부분이다.
		// 이외에도 Progressbar, Toolbar 등이 구현 대상이다.
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
			// 여기에서 크기를 제한시켜줘야하는데...
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

			// Style을 추가한 내용을 푸쉬하는 부분이다.
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
			// 이게 실제로 보여지는 부분이다.
			TickVisible();

			{
				ImGui::End();

				ImGui::PopStyleVar(m_varPushes);
				m_varPushes = 0;
			}
		}
	}
}