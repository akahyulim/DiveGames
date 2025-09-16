#include "GameView.h"
#include "../Editor.h"

namespace Dive
{
	GameView::GameView(Editor* editor)
		: View(editor)
	{
		m_title = "Game";
		m_flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground;
	}

	GameView::~GameView()
	{
	}

	void GameView::renderContent()
	{
		if (!EditorContext::ActiveWorld || !EditorContext::MainCamera)
			return;

		m_width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		m_height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		if (!m_renderTarget)
		{
			m_renderTarget = std::make_unique<RenderTexture>(
				static_cast<uint32_t>(m_width),
				static_cast<uint32_t>(m_height),
				eDepthFormat::Depth24Stencil8
			);
			m_renderTarget->Create();

			EditorContext::MainCamera->GetComponent<Camera>()->SetRenderTarget(m_renderTarget.get());
		}

		if (m_renderTarget->GetWidth() != static_cast<uint32_t>(m_width) || m_renderTarget->GetHeight() != static_cast<uint32_t>(m_height))
			m_renderTarget->Resize(static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));

		ImTextureID textureID = (ImTextureID)(m_renderTarget->GetShaderResourceView());
		ImGui::Image(textureID, ImVec2(m_width, m_height));
	}
}
