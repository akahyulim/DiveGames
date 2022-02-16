#include "ScenePanel.h"

ScenePanel::ScenePanel(Editor* pEditor)
	: Panel(pEditor, "Scene")
{
}

// Scene이 있다면
// Enigne의 Backbuffer에서 rtv를 얻어와 그린다.
void ScenePanel::renderWindow()
{
	// 크기를 계산한다. => 잘 모르겠다.
	auto width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
	auto height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

	// renderer의 viewport를 설정한다.
	// Editor용 Camera도 함께 설정??

	// 크기가 변경되었다면 renderer의 해상도를 변경한다.
	// 현재 MainRenderTarget을 변경하려 한다.
	//m_pRenderer->SetResolution(static_cast<unsigned int>(width), static_cast<unsigned int>(height));

	// image를 가져와 그린다.
	ImGui::Image(
		nullptr,//m_pSample,	// shader resource view다...
		ImVec2(width, height),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImColor(255, 255, 255, 255),
		ImColor(50, 127, 166, 255)
	);
}
