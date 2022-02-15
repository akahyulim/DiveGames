#include "ScenePanel.h"

ScenePanel::ScenePanel(Editor* pEditor)
	: Panel(pEditor, "Scene")
{
	m_pRenderer = &Dive::GetRenderer();
}

// Scene�� �ִٸ�
// Enigne�� Backbuffer���� rtv�� ���� �׸���.
void ScenePanel::renderWindow()
{
	if (!m_pRenderer)
		return;

	// ũ�⸦ ����Ѵ�. => �� �𸣰ڴ�.
	auto width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
	auto height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

	// renderer�� viewport�� �����Ѵ�.
	// Editor�� Camera�� �Բ� ����??

	// ũ�Ⱑ ����Ǿ��ٸ� renderer�� �ػ󵵸� �����Ѵ�.
	m_pRenderer->SetResolution(width, height);

	// image�� ������ �׸���.
	ImGui::Image(
		nullptr,	// shader resource view��...
		ImVec2(static_cast<float>(width), static_cast<float>(height)),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImColor(255, 255, 255, 255),
		ImColor(50, 127, 166, 255)
	);
}
