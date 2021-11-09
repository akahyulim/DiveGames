#include "Widget.h"
#include "Editor.h"

using namespace DiveEngine;

namespace DiveEditor
{
	Widget::Widget(Editor* pEditor)
	{
		assert(pEditor);

		m_pEditor = pEditor;

		EVENT_SUBSCRIBE(eEventType::SceneActivate, EVENT_HANDLE(OnSetActiveScene));
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

			if (m_Position.x != -1.0f && m_Position.y != -1.0f)
			{
				ImGui::SetNextWindowPos(m_Position);
			}
			if (m_Size.x != -1.0f && m_Size.y != -1.0f)
			{
				ImGui::SetNextWindowSize(m_Size, ImGuiCond_FirstUseEver);
			}
			// 여기에서 크기를 제한시켜줘야하는데...
			if ((m_Size.x != -1.0f && m_Size.y != -1.0f) || (m_MaxSize.x != FLT_MAX && m_MaxSize.y != FLT_MAX))
			{
				ImGui::SetNextWindowSizeConstraints(m_Size, m_MaxSize);
			}
			if (m_Alpha != -1.0f)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_Alpha);
				m_VarPushs++;
			}
			if (m_Padding.x != -1.0f && m_Padding.y != -1.0f)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_Padding);
				m_VarPushs++;
			}

			// Style을 추가한 내용을 푸쉬하는 부분이다.
			OnPushStyleVar();

			if (ImGui::Begin(m_Title.c_str(), &m_bVisible, m_Flags))
			{
				m_pWindow = ImGui::GetCurrentWindow();
				m_Height = ImGui::GetWindowHeight();

				bBegun = true;
			}
			else if (m_pWindow && m_pWindow->Hidden)
			{
				bBegun = true;
			}

			if (m_pWindow && m_pWindow->Appearing)
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

				ImGui::PopStyleVar(m_VarPushs);
				m_VarPushs = 0;
			}
		}
	}

	void Widget::OnSetActiveScene()
	{
		m_pScene = SceneManager::GetInstance().GetActiveScene();
	}
}