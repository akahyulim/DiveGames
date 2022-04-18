#include "ScenePanel.h"
#include "Base/SceneViewCamera.h"

ScenePanel::ScenePanel(Editor* pEditor)
	: Panel(pEditor, "Scene")
{
}

ScenePanel::~ScenePanel()
{
}

// Scene이 있다면
// Enigne의 Backbuffer에서 rtv를 얻어와 그린다.
void ScenePanel::renderWindow()
{
	if (!m_pActiveScene)
		return;

	// 크기를 계산한다. => 잘 모르겠다.
	auto width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
	auto height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

	// renderer의 viewport를 설정한다.
	// Editor용 Camera도 함께 설정??
	m_pEditor->GetSceneViewCamera()->SetViewportSize(width, height);

	// 이전 ContentRegion과의 비교가 필요하다.
	Dive::Renderer::SetTextures(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
	auto pTexture = Dive::Renderer::GetGbufferAlbedo();

	// 매번 그리는 게 맞다.
	ImGui::Image(
		pTexture ? pTexture->GetShaderResourceView() : nullptr,
		ImVec2(width, height),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImColor(255, 255, 255, 255),
		ImColor(50, 127, 166, 255)
	);
}
