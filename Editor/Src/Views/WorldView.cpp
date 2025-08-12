#include "WorldView.h"
#include "../MenuBar.h"
#include "../Editor.h"

namespace Dive
{
	WorldView::WorldView(Editor* editor)
		: View(editor)
	{
		m_title = "World";
		m_flags |= ImGuiWindowFlags_NoScrollbar;
	}

	WorldView::~WorldView()
	{
	}

	void WorldView::drawView()
	{
		if (!EditorContext::ActiveWorld || !EditorContext::EditorCamera)
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

			EditorContext::EditorCamera->GetComponent<Camera>()->SetRenderTarget(m_renderTarget.get());
		}

		if (m_renderTarget->GetWidth() != static_cast<uint32_t>(m_width) || m_renderTarget->GetHeight() != static_cast<uint32_t>(m_height))
			m_renderTarget->Resize(static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));

		ImTextureID textureID = (ImTextureID)(m_renderTarget->GetShaderResourceView());
		ImGui::Image(textureID, ImVec2(m_width, m_height));
	}
}
