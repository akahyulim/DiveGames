#include "ScenePanel.h"
#include "ImGuizmo/ImGuizmo.h"

namespace Editor
{
	ScenePanel::ScenePanel(Editor* pEditor)
		: Panel(pEditor, "Scene")
	{
		auto* pCache = m_pEditor->GetSubsystem<Dive::ResourceCache>();

		pCache->GetResource<Dive::Texture2D>("Assets/Textures/DokeV.jpeg");
		pCache->GetResource<Dive::Texture2D>("Assets/Textures/dmc.jpg");
		pCache->GetResource<Dive::Texture2D>("Assets/Textures/IU.jpg");
		pCache->GetResource<Dive::Texture2D>("Assets/Textures/ChoA.jpg");

		// ���� CreateScene �̺�Ʈ�� �����Ѵ�.
		// => ������������� ������ �����ε�, �̿� ���� m_pSceneRenderTarget�� ����� �� �� ����.
		// => �׷��� Scene�� 2�� �̻��� �� �ִ�.... Scene�̸����� ��Ī�� ��ų �� ���� �� ���� �ϴ�.
		m_pSceneRenderTarget = std::make_unique<Dive::Texture2D>(m_pEditor->GetContext());
		m_pSceneRenderTarget->SetSize(800, 600, DXGI_FORMAT_R8G8B8A8_UNORM, Dive::eTextureUsage::RenderTarget);
		m_pSceneViewport = new Dive::Viewport(m_pEditor->GetContext());
		m_pSceneRenderTarget->SetViewport(0, m_pSceneViewport);
	}

	ScenePanel::~ScenePanel()
	{
		DV_LOG_CLIENT_TRACE("ScenePanel �Ҹ��� ȣ��");
	}

	void ScenePanel::renderWindow()
	{
		if (!s_pActiveScene)
			return;

		auto width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		auto height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		// ������ ������ �߻��Ѵ�.
		// -> Graphics::ResetRenderTargets()�� ����������.
		if ((int)width != m_pSceneRenderTarget->GetWidth() || (int)height != m_pSceneRenderTarget->GetHeight())
		{
			m_pSceneRenderTarget->SetSize((int)width, (int)height, DXGI_FORMAT_R8G8B8A8_UNORM, Dive::eTextureUsage::RenderTarget);
		}

		ImGui::Image(
			m_pSceneRenderTarget ? m_pSceneRenderTarget->GetShaderResourceView() : nullptr,
			ImVec2(width, height),
			ImVec2(0, 0),
			ImVec2(1, 1),
			ImColor(255, 255, 255, 255),
			ImColor(50, 127, 166, 255)
		);
	}
}