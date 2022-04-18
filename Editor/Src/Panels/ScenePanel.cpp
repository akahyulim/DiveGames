#include "ScenePanel.h"
#include "Base/SceneViewCamera.h"

ScenePanel::ScenePanel(Editor* pEditor)
	: Panel(pEditor, "Scene")
{
}

ScenePanel::~ScenePanel()
{
}

// Scene�� �ִٸ�
// Enigne�� Backbuffer���� rtv�� ���� �׸���.
void ScenePanel::renderWindow()
{
	if (!m_pActiveScene)
		return;

	// ũ�⸦ ����Ѵ�. => �� �𸣰ڴ�.
	auto width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
	auto height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

	// renderer�� viewport�� �����Ѵ�.
	// Editor�� Camera�� �Բ� ����??
	m_pEditor->GetSceneViewCamera()->SetViewportSize(width, height);

	// ���� ContentRegion���� �񱳰� �ʿ��ϴ�.
	Dive::Renderer::SetTextures(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
	auto pTexture = Dive::Renderer::GetGbufferAlbedo();

	// �Ź� �׸��� �� �´�.
	ImGui::Image(
		pTexture ? pTexture->GetShaderResourceView() : nullptr,
		ImVec2(width, height),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImColor(255, 255, 255, 255),
		ImColor(50, 127, 166, 255)
	);
}
