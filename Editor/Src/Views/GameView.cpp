#include "GameView.h"

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
	}

	void GameView::drawView()
	{
		if (!Engine::GetWorld())
			return;

		m_Width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		m_Height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		static auto tex = Texture2D::LoadFromFile("NewProject/Assets/Textures/sky_daytime_blue.jpg", true);
		ImTextureID textureID = (ImTextureID)(tex ? tex->GetShaderResourceView() : nullptr);
		ImGui::Image(textureID, ImVec2(m_Width, m_Height));
	}
}
