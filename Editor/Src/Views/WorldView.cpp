#include "WorldView.h"
#include "../MenuBar.h"
#include "../Editor.h"

namespace Dive
{
	WorldView::WorldView(Editor* editor)
		: View(editor)
	{
		m_Title = "World";
		m_Flags |= ImGuiWindowFlags_NoScrollbar;
	}

	WorldView::~WorldView()
	{
	}

	void WorldView::drawView()
	{
		if (!EditorContext::ActiveWorld || !EditorContext::EditorCamera)
			return;

		m_Width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		m_Height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		if (!m_RenderTarget)
		{
			m_RenderTarget = std::make_unique<RenderTexture>(
				static_cast<UINT32>(m_Width), 
				static_cast<UINT32>(m_Height),
				eDepthFormat::Depth24Stencil8
		);
			m_RenderTarget->Create();

			EditorContext::EditorCamera->GetComponent<Camera>()->SetRenderTarget(m_RenderTarget.get());
		}

		if(m_RenderTarget->GetWidth() != static_cast<UINT32>(m_Width) || m_RenderTarget->GetHeight() != static_cast<UINT32>(m_Height))
				m_RenderTarget->Resize(static_cast<UINT32>(m_Width), static_cast<UINT32>(m_Height));

		ImTextureID textureID = (ImTextureID)(m_RenderTarget->GetShaderResourceView());
		ImGui::Image(textureID, ImVec2(m_Width, m_Height));
	}
}
