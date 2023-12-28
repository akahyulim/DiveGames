#include "SceneViewPanel.h"
#include "Dive.h"
#include "../ModelViewer.h"

#include <imgui-docking/imgui.h>
#include <Imgui-docking/imgui_internal.h>
#include <imgui-docking/imgui_stdlib.h>

SceneViewPanel::SceneViewPanel(ModelViewer* pModelViewer)
	: Panel(pModelViewer)
{
	m_PanelName = "SceneView";
}

// 창의 크기 변환시 크기값 갱신이 바로 이뤄지지 않는다.
void SceneViewPanel::OnRender()
{
	ImGui::Begin(GetPanelName().c_str(), 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

	uint32_t width = static_cast<uint32_t>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
	uint32_t height = static_cast<uint32_t>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

	static bool bFirstFrame = true;
	static uint32_t prevWidth = 0;
	static uint32_t prevHeight = 0;

	if (!bFirstFrame)
	{
		if (prevWidth != width || prevHeight != height)
		{
			Dive::Renderer::SetResolutionRender(width, height);

			prevWidth = width;
			prevHeight = height;
		}
	}
	bFirstFrame = false;
	
	auto pTex = Dive::Renderer::GetRenderTarget(Dive::eRenderTarget::FrameRender);
	ImGui::Image(pTex ? pTex->GetShaderResourceView() : nullptr, ImVec2(static_cast<float>(width), static_cast<float>(height)));

	Dive::Input::SetMouseIsInValidRect(ImGui::IsItemHovered());

	ImGui::End();
}
