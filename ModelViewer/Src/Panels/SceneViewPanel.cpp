#include "SceneViewPanel.h"
#include "Dive.h"

#include <imgui-docking/imgui.h>
#include <Imgui-docking/imgui_internal.h>
#include <imgui-docking/imgui_stdlib.h>

SceneViewPanel::SceneViewPanel()
	: Panel("SceneView")
{
}

SceneViewPanel::~SceneViewPanel()
{
}

void SceneViewPanel::OnRender()
{
	ImGui::Begin(GetPanelName().c_str(), 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

	auto width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
	auto height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

	if (Dive::Renderer::GetResolutionRender().x != width || Dive::Renderer::GetResolutionRender().y != height)
		Dive::Renderer::SetResolutionRender((uint32_t)width, (uint32_t)height);

	auto pTex = Dive::Renderer::GetRenderTarget(Dive::eRenderTarget::FrameRender);

	ImGui::Image(pTex ? pTex->GetShaderResourceView() : nullptr, ImVec2(width, height));

	Dive::Input::SetMouseIsInValidRect(ImGui::IsItemHovered());

	ImGui::End();
}
