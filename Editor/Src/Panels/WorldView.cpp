#include "WorldView.h"

namespace Dive
{
	WorldView::WorldView(Editor* pEditor)
		: Panel(pEditor)
	{
		m_Title = "WorldView";
	}

	void WorldView::renderWindow()
	{
		// ũ�⸦ ����Ѵ�. => �� �𸣰ڴ�.
		auto width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		auto height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		// srv�� �׸���. 
		// â�� ũ�⿡ ���� texture�� resize�ؾ� �Ѵ�.
		// renderer�� �ش� texture�� �����Ѵ�.

		ImGui::Image(
			nullptr,
			ImVec2(width, height));// ,
	//		ImVec2(0, 0),
	//		ImVec2(1, 1) ,
	//		ImColor(255, 255, 255, 255),
	//		ImColor(50, 127, 166, 255)
	//	);
	}
}