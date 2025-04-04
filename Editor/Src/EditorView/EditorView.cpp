#include "EditorView.h"

namespace Dive
{
	std::vector<std::shared_ptr<EditorView>> EditorView::m_Views;

	EditorView::EditorView()
		: m_Title("title")
		, m_Width(0)
		, m_Height(0)
		, m_bVisible(true)
	{
	}
	
	void EditorView::Draw()
	{
		if (!IsVisible())
			return;

		ImGui::Begin(m_Title.c_str(), &m_bVisible);// , ImGuiWindowFlags_ChildWindow);

		m_Width = ImGui::GetWindowWidth();
		m_Height = ImGui::GetWindowHeight();

		drawView();

		ImGui::End();
	}

	void EditorView::DrawViews()
	{
		for (auto& pView : m_Views)
		{
			pView->Draw();
		}
	}
}
