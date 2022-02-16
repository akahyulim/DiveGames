#include "ScenePanel.h"

ScenePanel::ScenePanel(Editor* pEditor)
	: Panel(pEditor, "Scene")
{
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

	// ũ�Ⱑ ����Ǿ��ٸ� renderer�� �ػ󵵸� �����Ѵ�.
	// ���� MainRenderTarget�� �����Ϸ� �Ѵ�.
	//m_pRenderer->SetResolution(static_cast<unsigned int>(width), static_cast<unsigned int>(height));

	// image�� ������ �׸���.
	ImGui::Image(
		nullptr,//m_pSample,	// shader resource view��...
		ImVec2(width, height),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImColor(255, 255, 255, 255),
		ImColor(50, 127, 166, 255)
	);
}
