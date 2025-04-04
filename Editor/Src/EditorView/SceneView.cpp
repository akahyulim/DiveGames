#include "SceneView.h"
#include "HierarchyView.h"

namespace Dive
{
	SceneView::SceneView()
		: m_pRenderTarget(nullptr)
	{
		m_Title = "World";

		// 유니티의 경우 카메라에서 렌더타겟을 가진다.
		// 따라서 카메라별 리소스 뷰를 가져와서 그릴 수 있다.
		// 좀 더 자세히 설명하자면 카메라의 렌더타겟이 백버퍼 바로 전에 출력되는 타겟이다.
		// 실제로 Camera.Render()가 존재하는 듯 한데 이는 카메라 기준으로 렌더링이 수행된다고 볼 수도 있다.
		// => Camera의 Render()는 자동으로 호출되나 사용자가 특정 시점에 직접 호출할 수도 있다.
		// => 이 경우 호출이 중복될 수 있는데 이를 피하는 플래그가 존재한다.
		// + Camera가 렌더타겟을 가지지 않을 수도 있다. 이 경우 백버퍼에 렌더링을 수행한다.
	}

	SceneView::~SceneView()
	{
		DV_DELETE(m_pRenderTarget);
	}

	void SceneView::drawView()
	{
		m_Width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		m_Height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		if (!m_pRenderTarget || m_pRenderTarget->GetWidth() != static_cast<uint32_t>(m_Width) || m_pRenderTarget->GetHeight() != static_cast<uint32_t>(m_Height))
		{
			DV_DELETE(m_pRenderTarget);
			m_pRenderTarget = new Texture2D(static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height));
		}

		if (Editor::GetInstance().GetActiveWorld())
		{
			Editor::GetInstance().GetEditorCamera()->GetComponent<Camera>()->SetRenderTargetTex(m_pRenderTarget);
		}

		// 테스트용 클리어
		{
			const float clear_color_with_alpha[4]{ 1.0f, 0.0f, 1.0f, 1.0f };
			Graphics::GetDeviceContext()->ClearRenderTargetView(m_pRenderTarget->GetRenderTargetView(), clear_color_with_alpha);
		}

		ImTextureID textureID = (ImTextureID)(m_pRenderTarget ? m_pRenderTarget->GetShaderResourceView() : nullptr);
		ImGui::Image(textureID, ImVec2(m_Width, m_Height));
	}
}
