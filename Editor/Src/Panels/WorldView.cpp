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
		// 크기를 계산한다. => 잘 모르겠다.
		auto width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		auto height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		// srv를 그린다. 
		// 창의 크기에 맞춰 texture를 resize해야 한다.
		// renderer가 해당 texture를 관리한다.

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