#include "EditorRenderPath.h"
#include "SceneViewCamera.h"

void EditorRenderPath::Update(float delta)
{
	if (!m_pActiveScene || !m_pSceneViewCamera)
		return;

	// 둘 다 위치가 애매하다. 
	// 그렇다고 Renderer에 Update를 두는 것도 이상한데...
	m_pSceneViewCamera->Update(delta);
	m_pActiveScene->Update(delta);

	m_MainVisibilities;
	m_MainVisibilities.pScene = m_pActiveScene;
	Dive::Renderer::UpdateVisibility(m_MainVisibilities);
}

void EditorRenderPath::Render()
{
	if (!m_pActiveScene || !m_pSceneViewCamera)
		return;

	// begin scene
	// : 현재 직접 bind하고 있다. 
	{
		// 이것도 Pass 안에 넣어야 할까?
		// 스파르탄의 경우 초기화는 외부에서 하고
		// pass에 필요한 것들을 인자로 전달한 후 bind하는 것 같다.
		auto pImmediateContext = Dive::Renderer::GetGraphicsDevice().GetImmediateContext();
		auto pRenderTargetView = Dive::Renderer::GetGBufferAlbedo()->GetRenderTargetView();
		auto pDepthStencilView = Dive::Renderer::GetDepthStencilTexture()->GetDepthStencilView();
		if (!pImmediateContext || !pRenderTargetView)
			return;

		float clearColors[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

		pImmediateContext->ClearRenderTargetView(pRenderTargetView, clearColors);
		if (pDepthStencilView)
			pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		UINT numViews = 8;
		std::array<ID3D11RenderTargetView*, 8> renderTargetViews = { nullptr };
		renderTargetViews[0] = pRenderTargetView;
		//pImmediateContext->OMSetRenderTargets(1, renderTargetViews.data(), pDepthStencilView);
		//pImmediateContext->RSSetViewports(1, Dive::Renderer::GetGBufferAlbedo()->GetViewport());
	}

	Dive::CommandList cl;

	// 카메라의 역할은 이게 가장 크다.
	// 이외에도 클리핑 기능이 있긴 한데... 그게 viewport일까? 아니면 frustum일까?
	// 어쨌든간에 pass안에 넣는게 나아보인다.
	// 그렇다면 결국 SceneViewCamera는 Camera를 상속해야 한다.
	auto view = m_pSceneViewCamera->GetViewMatrix();
	auto proj = m_pSceneViewCamera->GetProjectionMatrix();
	auto eyePos = m_pSceneViewCamera->GetPosition();

	auto pImmediateContext = Dive::Renderer::GetGraphicsDevice().GetImmediateContext();

	// 매 프레임 한 번만 bind하면 모든 Shader에서 사용할 수 있다.
	// FrameBuffer
	{
		auto pCbFrame = Dive::Renderer::GetCbFrame();

		// map & unmap
		auto pPtr = static_cast<Dive::FrameBuffer*>(pCbFrame->Map());
		pPtr->view = DirectX::XMMatrixTranspose(view);
		pPtr->proj = DirectX::XMMatrixTranspose(proj);
		
		{
			// linear depth 계산에 사용된다.
			// 정확히 무슨 값인지 아직 모른다.
			DirectX::XMFLOAT4X4 p;
			DirectX::XMStoreFloat4x4(&p, proj);
			pPtr->perspectiveValues.x = 1.0f / p._11;
			pPtr->perspectiveValues.y = 1.0f / p._22;
			pPtr->perspectiveValues.z = p._43;
			pPtr->perspectiveValues.w = -p._33;
		}

		pPtr->eyePos = eyePos;
		pCbFrame->Unmap();

		cl.SetConstantBuffer(Dive::Scope_Vertex | Dive::Scope_Pixel, Dive::eConstantBufferSlot::Frame, pCbFrame);
	}

	// 일단 하드 코딩
	auto pPointSampler = Dive::Renderer::GetSamplerState(Dive::eSamplerStateType::Point);
	auto pLinearSampler = Dive::Renderer::GetSamplerState(Dive::eSamplerStateType::Linear);
	pImmediateContext->PSSetSamplers(0, 1, &pPointSampler);
	pImmediateContext->PSSetSamplers(1, 1, &pLinearSampler);

	//Dive::RenderPath::passDefault(&cl);
	Dive::RenderPath::passGBuffer(&cl);
	Dive::RenderPath::passLighting(&cl);

	// BeginScene은 사실상 구현하기 애매하니,
	// EndScene보다 Present라는 이름이 어울린다.
	// 그런데 이 곳에서 호출하면 안된다.
	// ImGui도 Present할 때 문제가 발생한다.
	//Dive::Renderer().EndScene();
}
