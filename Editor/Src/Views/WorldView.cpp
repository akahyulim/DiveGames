#include "WorldView.h"
#include "../MenuBar.h"

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
		/*
		if (!WorldManager::GetActiveWorld())
			return;

		m_Width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		m_Height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;
		
		auto editorCamera = MenuBar::GetEditorCamera();
		if (editorCamera)
		{
			m_RenderTexture = editorCamera->GetRenderTexture();
			if (m_RenderTexture && m_RenderTexture->GetWidth() != static_cast<UINT32>(m_Width) || m_RenderTexture->GetHeight() != static_cast<UINT32>(m_Height))
				m_RenderTexture->Resize(static_cast<UINT32>(m_Width), static_cast<UINT32>(m_Height));
		}

		// 이 부분은 기본적으로 Renderer에서 수행해야 한다.
		{
			auto renderTargetView = m_RenderTexture->GetRenderTargetView();
			Graphics::GetDeviceContext()->OMSetRenderTargets(1, &renderTargetView, nullptr);
			FLOAT clearColor[4]{editorCamera->GetBackgroundColor().x, editorCamera->GetBackgroundColor().y,
				editorCamera->GetBackgroundColor().z, editorCamera->GetBackgroundColor().w };
			Graphics::GetDeviceContext()->ClearRenderTargetView(renderTargetView, clearColor);
		}

		static auto tex = Texture2D::LoadFromFile("NewProject/Assets/Textures/relaxed_morning.jpg", true);
		ImTextureID textureID = (ImTextureID)(m_RenderTexture ? m_RenderTexture->GetShaderResourceView() : tex->GetShaderResourceView());
		ImGui::Image(textureID, ImVec2(m_Width, m_Height));
		*/
	}
}
