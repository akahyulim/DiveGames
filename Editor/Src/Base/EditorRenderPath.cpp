#include "EditorRenderPath.h"
#include "SceneViewCamera.h"

void EditorRenderPath::Update(float delta)
{
	if (!m_pActiveScene || !m_pSceneViewCamera)
		return;

	m_pSceneViewCamera->Update(delta);
	m_pActiveScene->Update(delta);	// 이건 위치가 좀 애매하다.

	m_MainVisibilities;
	m_MainVisibilities.pScene = m_pActiveScene;

	Dive::Renderer::UpdateVisibility(m_MainVisibilities);
}

void EditorRenderPath::Render()
{
	if (!m_pActiveScene || !m_pSceneViewCamera)
		return;

	// begin scene
	{
		auto pImmediateContext = Dive::Renderer::GetGraphicsDevice().GetImmediateContext();
		auto pRenderTargetView = Dive::Renderer::GetSampleTexture()->GetRenderTargetView();
		auto pDepthStencilView = Dive::Renderer::GetDepthStencilTexture()->GetDepthStencilView();
		if (!pImmediateContext || !pRenderTargetView)
			return;

		// active camera로부터 가져와야 한다.
		float clearColors[4] = { 0.35f, 0.35f, 0.7f, 1.0f };

		pImmediateContext->ClearRenderTargetView(pRenderTargetView, clearColors);
		if (pDepthStencilView)
			pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView ? pDepthStencilView : nullptr);

		// 좀 애매하다.
		D3D11_VIEWPORT viewport;
		viewport.Width = (float)Dive::Renderer::GetSampleTexture()->GetWidth();
		viewport.Height = (float)Dive::Renderer::GetSampleTexture()->GetHeight();
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		pImmediateContext->RSSetViewports(1, &viewport);
	}

	Dive::CommandList cl;

	auto view = m_pSceneViewCamera->GetViewMatrix();
	auto proj = m_pSceneViewCamera->GetProjectionMatrix();

	Dive::RenderPath::passDefault(&cl, view, proj);
}
