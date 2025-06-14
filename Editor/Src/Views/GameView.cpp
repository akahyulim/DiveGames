#include "GameView.h"
#include "../Editor.h"

namespace Dive
{
	GameView::GameView(Editor* editor)
		: View(editor)
	{
		m_Title = "Game";
		m_Flags |= ImGuiWindowFlags_NoScrollbar;
	}

	GameView::~GameView()
	{
		DV_DELETE(m_RenderTarget);
	}

	void GameView::drawView()
	{
		auto& editorContext = EditorContext::GetInstance();
		if (!editorContext.ActiveWorld || editorContext.MainCamera == entt::null)
			return;

		m_Width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		m_Height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		if (!m_RenderTarget)
		{
			m_RenderTarget = new RenderTexture(static_cast<UINT32>(m_Width), static_cast<UINT32>(m_Height));
			m_RenderTarget->Create();

			//auto& cameraCom = editorContext.ActiveWorld->GetEntityManager().GetComponent<CameraComponent>(editorContext.MainCamera);
			//if (cameraCom.RenderTarget != m_RenderTarget)
			//	cameraCom.RenderTarget = m_RenderTarget;
		}
		else
		{
			if (m_RenderTarget->GetWidth() != static_cast<UINT32>(m_Width) || m_RenderTarget->GetHeight() != static_cast<UINT32>(m_Height))
				m_RenderTarget->Resize(static_cast<UINT32>(m_Width), static_cast<UINT32>(m_Height));
		}

		auto dmc = ResourceManager::GetByPath<Texture2D>("Textures/dmc.jpg");
		ImTextureID textureID = (ImTextureID)(dmc ? dmc->GetShaderResourceView() : m_RenderTarget->GetShaderResourceView());
		ImGui::Image(textureID, ImVec2(m_Width, m_Height));
	}
}
