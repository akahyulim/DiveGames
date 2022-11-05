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

		// 역시 CreateScene 이벤트를 구독한다.
		// => 리뉴얼과정에서 제거한 구문인데, 이에 맞춰 m_pSceneRenderTarget을 만들면 될 것 같다.
		// => 그런데 Scene은 2개 이상을 수 있다.... Scene이름으로 매칭을 시킬 수 있을 것 같긴 하다.
		m_pSceneRenderTarget = std::make_unique<Dive::Texture2D>(m_pEditor->GetContext());
		m_pSceneRenderTarget->SetSize(800, 600, DXGI_FORMAT_R8G8B8A8_UNORM, Dive::eTextureUsage::RenderTarget);
		m_pSceneViewport = new Dive::Viewport(m_pEditor->GetContext());
		m_pSceneRenderTarget->SetViewport(0, m_pSceneViewport);
	}

	ScenePanel::~ScenePanel()
	{
		DV_LOG_CLIENT_TRACE("ScenePanel 소멸자 호출");
	}

	void ScenePanel::renderWindow()
	{
		if (!s_pActiveScene)
			return;

		auto width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		auto height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		// 아직도 문제가 발생한다.
		// -> Graphics::ResetRenderTargets()후 괜찮아졌다.
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