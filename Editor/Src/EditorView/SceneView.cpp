#include "SceneView.h"
#include "HierarchyView.h"

namespace Dive
{
	SceneView::SceneView()
		: m_pRenderTarget(nullptr)
	{
		m_Title = "World";

		// ����Ƽ�� ��� ī�޶󿡼� ����Ÿ���� ������.
		// ���� ī�޶� ���ҽ� �並 �����ͼ� �׸� �� �ִ�.
		// �� �� �ڼ��� �������ڸ� ī�޶��� ����Ÿ���� ����� �ٷ� ���� ��µǴ� Ÿ���̴�.
		// ������ Camera.Render()�� �����ϴ� �� �ѵ� �̴� ī�޶� �������� �������� ����ȴٰ� �� ���� �ִ�.
		// => Camera�� Render()�� �ڵ����� ȣ��ǳ� ����ڰ� Ư�� ������ ���� ȣ���� ���� �ִ�.
		// => �� ��� ȣ���� �ߺ��� �� �ִµ� �̸� ���ϴ� �÷��װ� �����Ѵ�.
		// + Camera�� ����Ÿ���� ������ ���� ���� �ִ�. �� ��� ����ۿ� �������� �����Ѵ�.
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

		// �׽�Ʈ�� Ŭ����
		{
			const float clear_color_with_alpha[4]{ 1.0f, 0.0f, 1.0f, 1.0f };
			Graphics::GetDeviceContext()->ClearRenderTargetView(m_pRenderTarget->GetRenderTargetView(), clear_color_with_alpha);
		}

		ImTextureID textureID = (ImTextureID)(m_pRenderTarget ? m_pRenderTarget->GetShaderResourceView() : nullptr);
		ImGui::Image(textureID, ImVec2(m_Width, m_Height));
	}
}