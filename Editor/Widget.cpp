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
		// 이건 이름처럼 항상 그려지는 부분이고
		TickAlways();

		if (!m_bWindow)	
			return;

		// 아래의 부분은 특정 상황??? 에서만 보여지는 것 같다.
		// MenuBar로는 재현이 불가능하다.
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

			// 순서상 구체 위젯이 먼저 그려진 후 호출된다.
			// 따라서 결국 다른 윈도우를 만들어 그리는 형태가 되었다...
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