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
		DV_DELETE(m_RenderTarget);
	}

	void WorldView::drawView()
	{
		auto& editorContext = EditorContext::GetInstance();
		if (!editorContext.ActiveWorld || editorContext.EditorCamera == entt::null)
			return;

		m_Width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		m_Height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		if (!m_RenderTarget)
		{
			m_RenderTarget = new RenderTexture(static_cast<UINT32>(m_Width), static_cast<UINT32>(m_Height));
			m_RenderTarget->Create();

			auto& cameraCom = editorContext.ActiveWorld->GetEntityManager().GetComponent<CameraComponent>(editorContext.EditorCamera);
			if (cameraCom.RenderTarget != m_RenderTarget)
				cameraCom.RenderTarget = m_RenderTarget;
		}
		else
		{
			if(m_RenderTarget->GetWidth() != static_cast<UINT32>(m_Width) || m_RenderTarget->GetHeight() != static_cast<UINT32>(m_Height))
				m_RenderTarget->Resize(static_cast<UINT32>(m_Width), static_cast<UINT32>(m_Height));
		}

		auto DokeV = ResourceManager::GetByName<Texture2D>("DokeV");
		ImTextureID textureID = (ImTextureID)(DokeV ? DokeV->GetShaderResourceView() : m_RenderTarget->GetShaderResourceView());
		ImGui::Image(textureID, ImVec2(m_Width, m_Height));
	}
}
