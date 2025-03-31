#include "GameView.h"

namespace Dive
{
	GameView::GameView()
	{
		m_Title = "Game";

		m_pTex = GResourceManager->Load<Texture2D>("../../Assets/Textures/DokeV.jpeg");
	}

	void GameView::drawView()
	{
		m_Width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		m_Height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		// srv�� �׸���. 
		// â�� ũ�⿡ ���� texture�� resize�ؾ� �Ѵ�.
		// renderer�� �ش� texture�� �����Ѵ�.
		//GEngine->GetRenderer().ResizeTargets(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		ImTextureID textureID = (ImTextureID)( m_pTex ? m_pTex->GetShaderResourceView() : nullptr);
		ImGui::Image(textureID, ImVec2(m_Width, m_Height));
	}
}