#include "ScenePanel.h"

ScenePanel::ScenePanel(Editor* pEditor)
	: Panel(pEditor, "Scene")
{
	m_pSampleTex = Dive::Texture2D::Create("Assets/Textures/DokeV.jpeg");
}

ScenePanel::~ScenePanel()
{
	DV_DELETE(m_pSampleTex);
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

	// 이전 ContentRegion과의 비교가 필요하다.
	Dive::GetRenderer().SetTextures(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
	auto pTexture = Dive::GetRenderer().GetSampleTexture();

	// 매번 그리는 게 맞다.
	ImGui::Image(
		pTexture ? pTexture->GetShaderResourceView() : (m_pSampleTex ? m_pSampleTex->GetShaderResourceView() : nullptr),
		ImVec2(width, height),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImColor(255, 255, 255, 255),
		ImColor(50, 127, 166, 255)
	);
}
