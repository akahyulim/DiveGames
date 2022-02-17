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

// Scene�� �ִٸ�
// Enigne�� Backbuffer���� rtv�� ���� �׸���.
void ScenePanel::renderWindow()
{
	// ũ�⸦ ����Ѵ�. => �� �𸣰ڴ�.
	auto width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
	auto height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

	// renderer�� viewport�� �����Ѵ�.
	// Editor�� Camera�� �Բ� ����??

	// ���� ContentRegion���� �񱳰� �ʿ��ϴ�.
	Dive::GetRenderer().SetTextures(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
	auto pTexture = Dive::GetRenderer().GetSampleTexture();

	// �Ź� �׸��� �� �´�.
	ImGui::Image(
		pTexture ? pTexture->GetShaderResourceView() : (m_pSampleTex ? m_pSampleTex->GetShaderResourceView() : nullptr),
		ImVec2(width, height),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImColor(255, 255, 255, 255),
		ImColor(50, 127, 166, 255)
	);
}
